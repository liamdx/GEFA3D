#define _CRT_SECURE_NO_WARNINGS


#include "Common.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "MeshCollider.h"
#include "GameObject.h"
#include "primitives\Cube.h"
#include "filesystem.h"
#include "lighting\DirectionalLight.h"
#include "lighting\PointLight.h"
#include "lighting\SpotLight.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw_gl3.h"

//Engine Time
float deltaTime = 0.0f;
float lastFrame = 0.0f;
const float timeStep = 1 / 60.0f;

//camera;
float lastX;
float lastY;
bool firstMouse = true;
Camera cam(glm::vec3(0.0f, 0.0f, 3.0f));

//Forward Decalrations
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//lights
PointLight pointLights[4];
SpotLight spotLights[4];

const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;

// We don't care for a profiler. This definition does nothing.
void b3BeginProfileScope(const char* name)
{

}

// We don't care for a profiler. This definition does nothing.
void b3EndProfileScope()
{

}


int main(void)
{
	//DEBUG
	int  success;
	char infoLog[512];
	//GL initialisation
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GEFA3D", NULL, NULL);
	
	//Does nothing until a frame buffer is set up
	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "failed to initalise glew" << std::endl;
	}

	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	//Mouse input handle

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	//Enable depth
	glEnable(GL_DEPTH_TEST);

	//Physics
	//init
	// The world gravity.
	const b3Vec3 gravity(0.0f, -9.8f, 0.0f);
	// The fixed time step size.
	const float32 timeStep = 1.0f / 60.0f;
	// Number of iterations for the velocity constraint solver.
	const u32 velocityIterations = 8;
	// Number of iterations for the position constraint solver.
	const u32 positionIterations = 2;

	//Create Physics world
	// The world-> We allocate it using the heap but you can to it 
	// on the stack if the stack is sufficiently large.
	b3World* world = new b3World();
	world->SetGravity(gravity);

	// Create a static ground body at the world origin.
	b3BodyDef groundDef;
	b3Body* ground = world->CreateBody(groundDef);
	// Create a box positioned at the world origin and 
	// aligned with the world frame.
	b3BoxHull groundBox;
	// Set the ground box dimensions using a linear scale transform.
	b3Transform scale;
	scale.position.SetZero();
	scale.rotation = b3Diagonal(100.0f, 1.0f, 100.0f);
	groundBox.SetTransform(scale);
	// Create the box physics wrapper.
	b3HullShape groundShape;
	groundShape.m_hull = &groundBox;
	// Add the box to the ground body.
	b3ShapeDef groundBoxDef;
	groundBoxDef.shape = &groundShape;
	ground->CreateShape(groundBoxDef);
	// Create a dynamic body.
	b3BodyDef bodyDef;
	bodyDef.type = e_dynamicBody;
	bodyDef.fixedRotationX = true;
	bodyDef.fixedRotationZ = true;
	// Position the body 10 meters high from the world origin.
	bodyDef.position.Set(0.0f, 100.0f, 0.0f);
	// Set the initial angular velocity to pi radians (180 degrees) per second.
	bodyDef.angularVelocity.Set(0.0f, B3_PI, 0.0f);
	b3Body* body = world->CreateBody(bodyDef);
	// Create a unit box positioned at the world origin and 
	// aligned with the world frame.
	b3BoxHull bodyBox;
	bodyBox.SetIdentity();
	// Create the box physics wrapper.
	b3HullShape bodyShape;
	bodyShape.m_hull = &bodyBox;

	std::string nanoPath = "C:/Users/lenovo/Documents/GitHub/GEFA3D/GEFA3D/res/nanosuit/nanosuit.obj";
	Model nano(nanoPath);
	
	std::string terrainPath = "C:/Users/lenovo/Documents/GitHub/GEFA3D/GEFA3D/res/sponza_obj/sponza.obj";
	Model terrain(terrainPath);

	b3Capsule capsule = b3Capsule_identity;
	capsule.radius = 2.0f;
	b3CapsuleShape capsuleShape;
	capsuleShape.m_centers[0].Set(0.0f, 0.5f, 0.0f);
	capsuleShape.m_centers[1].Set(0.0f, -0.5f, 0.0f);
	capsuleShape.m_radius = 2.0f;


	// Add the box to the body.
	b3ShapeDef bodyBoxDef;
	bodyBoxDef.shape = &capsuleShape;
	bodyBoxDef.density = 10.0f;
	body->CreateShape(bodyBoxDef);


	Shader lightShader("res/light.vert", "res/light.frag");
	Shader lampShader("res/lamp.vert", "res/lamp.frag");

	Cube cube("C:/Users/lenovo/Documents/GitHub/GEFA3D/GEFA3D/res/green.jpg");
	GameObject crysisObject(nanoPath, lightShader, world);

	//mvp

	glm::mat4 view = glm::mat4(1.0f);
	view = cam.GetViewMatrix();
	//model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//projection
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(cam.Zoom, (float)SCREEN_WIDTH / SCREEN_HEIGHT , 0.1f, 100.0f);
	
	glm::mat4 mvp = glm::mat4(1.0f);
	mvp = projection * view * model;


	//Texture diffuseTexture("res/container2.png",0);
	//Texture specularTexture("res/container2_specular.png", 1);


	
	float timeValue;
	float offsetValue;

	glm::vec3 cubePositions[]{
		glm::vec3(0.0f,0.0f,0.0f),
		glm::vec3(1.0f,2.0f,-3.0f),
		glm::vec3(-2.0f,4.0f,-6.0f),
		glm::vec3(5.0f,10.6f,-9.0f),
		glm::vec3(3.0f,5.0f,-12.0f),
		glm::vec3(6.0f,7.0f,0.0f),
		glm::vec3(3.0f,6.0f,-7.70f),
		glm::vec3(-5.0f,4.0f,-6.0f),
		glm::vec3(7.0f,0.6f,-9.0f),
		glm::vec3(-3.0f,0.0f,-15.0f)
	};


	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	//DirectionalLight dirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.14f, 0.13f, 0.14f) ,glm::vec3(0.2f, 0.12f, 0.13f),glm::vec3(0.2f, 0.2f, 0.2f));
	DirectionalLight dirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.01f, 0.01f, 0.01f), glm::vec3(0.13f, 0.04f, 0.01f), glm::vec3(1.01f, 1.01f, 1.01f));
	attenuation att = { 1.0f, 0.22f, 0.2f };
	PointLight pl = PointLight(glm::vec3(1.0), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.70f, 0.25f, 0.25f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, att);


	

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec4 lightposition = ImVec4(0.571f, 10.286f, 2.60f, 1.00f);
	ImVec4 lightambient = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);
	ImVec4 lightdiffuse = ImVec4(3.429f, 2.0f, 3.429f, 1.00f);
	ImVec4 lightspecular = ImVec4(3.429f, 2.0f, 3.429f, 1.00f);
	ImVec4 lightattenuation = ImVec4(1.0, 0.22f, 0.20f, 1.00f);
	float lightintensity = 1.0f;
	ImVec4 nanoposition = ImVec4(0.45f, 0.0f, 0.60f, 1.00f);
	ImVec4 testposition = ImVec4(0.45f, 2.0f, 0.60f, 1.00f);

	cam.Position = glm::vec3(0.0, 9.0, 10.0);
	/* Loop until the user closes the window */
				//LOOP//
	
	while (!glfwWindowShouldClose(window))
	{
		processInput(window); 
		timeValue = glfwGetTime();
		offsetValue = (sin(timeValue) / 2.0f);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// Perform a time step of the world in this frame.
		world->Step(deltaTime, velocityIterations, positionIterations);
		ImGui_ImplGlfwGL3_NewFrame();
		//world.update(deltaTime);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		//glClearColor(0.701f, 0.839f, 0.972f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		/* Render here */
		//glClearColor(0.678f, 0.847f, 0.901f, 1.0f);
		//glClearColor(0.0f, 0.749f, 0.647f, 1.0f);
		//glClearColor(0.18f, 0.02f, 0.05f, 0.4f);
		//glClearColor(0.701f, 0.839f, 0.972f, 1.0f);
	

		lightShader.use();
		lightShader.setInt("NUMBER_OF_TEXTURES", 1);
		lightShader.setInt("NOPL", 1);
		lightShader.setInt("NOSL", 0);
		lightShader.setVec3("viewPosition", cam.Position);
		lightShader.setFloat("mat.m_Shininess", 64.0f);

		//change to include 1 dirLight , 4 point lights, 1 spot
		/*lightShader.setVec3("light.l_Ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		lightShader.setVec3("light.l_Diffuse", glm::vec3(0.6f, 0.6f, 0.6f)); // darken the light a bit to fit the scene
		lightShader.setVec3("light.l_Specular", glm::vec3(0.508273f, 0.508273f, 0.508273f));
		lightShader.setFloat("light.innerCutoff", glm::cos(glm::radians(20.0f)));
		lightShader.setFloat("light.outerCutoff", glm::cos(glm::radians(25.0f)));
		lightShader.setFloat("light.constant", 1.0f);
		lightShader.setFloat("light.linear", 0.09f);
		lightShader.setFloat("light.quadratic", 0.032f);
		*/

		dirLight.Bind(lightShader);
		view = cam.GetViewMatrix();

		//render lights
		//for (int i = 0; i < 1; i++)
		//{
		//	pointLights[i] = PointLight(glm::vec3(lightposition.x , lightposition.y , lightposition.z), glm::vec3(0.01f, 0.01f, 0.01f), glm::vec3(10.0f, 10.25f, 10.25f), glm::vec3(10.0f, 10.0f, 10.0f), att);
		//	pointLights[i].Bind(lightShader, i);
		//	spotLights[i] = SpotLight(glm::vec3(0.25f * i), glm::vec3(0.01f *i, 0.1f * i, 1.0 * i), 15.0f, 20.0f, glm::vec3(0.01f, 0.01f, 0.01f), glm::vec3(0.01f, 0.01f, 0.01f), glm::vec3(0.0f, 0.0f, 0.0f), att);
		//	spotLights[i].Bind(lightShader, i);
		//}
		
		pl.setPosition(glm::vec3(lightposition.x, lightposition.y, lightposition.z));
		pl.setAmbient(glm::vec3(lightambient.x, lightambient.y, lightambient.z));
		pl.setDiffuse(glm::vec3(lightdiffuse.x, lightdiffuse.y, lightdiffuse.z));
		pl.setSpecular(glm::vec3(lightspecular.x, lightspecular.y, lightspecular.z));
		pl.setAttenuation(glm::vec3(lightattenuation.x, lightattenuation.y, lightattenuation.z));
		pl.setIntensity(lightintensity);

		pl.Bind(lightShader);
		
		
		b3Vec3 position = body->GetPosition();
		b3Quat orientation = body->GetOrientation();
		b3Vec3 axis;
		float32 angle;
		orientation.GetAxisAngle(&axis, &angle);
		lightShader.use();
		model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(nanoposition.x, nanoposition.y, nanoposition.z));
		model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
		model = glm::rotate(model, angle, glm::vec3(axis.x, axis.y,axis.z));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		lightShader.setMat4("model", model);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
		nano.Draw(lightShader);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 3.5f, 5.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		mvp = projection * view * model;
		lightShader.setMat4("model", model);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
		terrain.Draw(lightShader);

		crysisObject.g_Transform.position = glm::vec3(testposition.x, testposition.y, testposition.z);
		crysisObject.Draw();

		lampShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(lightposition.x, lightposition.y, lightposition.z));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		mvp = projection * view * model;
		lampShader.setMat4("model", model);
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);
		lampShader.setVec3("color", pl.diffuse);
		cube.Draw(lampShader);

		//ui stuff
		// 1. Show a simple window.
		// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
		{
			static float f = 0.0f;
			static int counter = 0;
			ImGui::Text("Debug");                           // Display some text (you can use a format string too)        // Edit 1 float using a slider from 0.0f to 1.0f    
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
			//ImGui::SliderFloat3("suit pos", &nanoposition.x, -10, 10);
			ImGui::SliderFloat3("light pos", &lightposition.x, -100, 100);
			ImGui::SliderFloat3("mario pos", &testposition.x, -100, 100);
			ImGui::SliderFloat("Light intensity", &lightintensity, 0, 25);
			ImGui::ColorEdit3("light ambient", (float*)&lightambient);
			ImGui::ColorEdit3("light diffuse", (float*)&lightdiffuse);
			ImGui::ColorEdit3("light specular", (float*)&lightspecular);
			ImGui::InputFloat3("light attenuation", &lightattenuation.x, -2, 2);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			if (ImGui::Button("addForce"))
			{
				body->ApplyForce(b3Vec3(10000, 10000.0f, 0), b3Vec3(1, 0, 0), true);
			}
		}

		//ui render
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		


		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	// Cleanup
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		if (cam.canMove) {
			cam.canMove = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else if (cam.canMove == false)
		{
			cam.canMove = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

		
		



}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	cam.ProcessMouseMovement(xoffset, yoffset);
};

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cam.ProcessMouseScroll(yoffset);
};
