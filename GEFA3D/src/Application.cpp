#define _CRT_SECURE_NO_WARNINGS

//Libs, helper functions and commonly used code
#include "Common.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "MeshCollider.h"
#include "Object.h"
#include "primitives\Cube.h"
#include "primitives\Quad.h"
#include "lighting\DirectionalLight.h"
#include "lighting\PointLight.h"
#include "lighting\SpotLight.h"
#include "ext/imgui/imgui.h"
#include "ext/imgui/imgui_impl_glfw_gl3.h"
#include "DebugEngineComponent.h"
#include "Transform.h"
#include "Cubemap.h"

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
std::vector<PointLight> pointLights;
std::vector<SpotLight> spotLights;


const unsigned int SCREEN_WIDTH = 1600;
const unsigned int SCREEN_HEIGHT = 900;

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

	//Does nothing? what needs to be set up / is fxaa a better solution?
	//glfwWindowHint(GLFW_SAMPLES, 4);
	//glEnable(GL_MULTISAMPLE);

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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(window, mouse_callback);

	//Enable depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


	glEnable(GL_PROGRAM_POINT_SIZE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	cam.MovementSpeed = 10.0f;
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

	std::string nanoPath = "res/models/nanosuit_reflection/nanosuit.obj";
	Model nano(nanoPath);
	std::string terrainPath = "res/models/sponza_obj/sponza.obj";
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

	//Test objects

	Shader lightShader("res/shaders/light.vert", "res/shaders/light.frag");
	Shader lampShader("res/shaders/lamp.vert", "res/shaders/lamp.frag");
	Shader outlineShader("res/shaders/default.vert", "res/shaders/outline.frag");
	Shader grassShader("res/shaders/default.vert", "res/shaders/grass.frag");
	Shader fbShader("res/shaders/FrameBuffer.vert", "res/shaders/FrameBuffer.frag");
	Shader cubemapShader("res/shaders/Cubemap.vert", "res/shaders/Cubemap.frag");
	Shader reflectionShader("res/shaders/Reflective.vert", "res/shaders/Reflective.frag");
	Shader reflectionMapShader("res/shaders/Default.vert", "res/shaders/Default.frag");

	Cube cube("C:/Users/Liam/Documents/GitHub/GEFA3D/GEFA3D/res/textures/green.jpg");
	Cube reflectiveCube;
	Quad quad("res/textures/grass.png");
	screenQuad renderQuad;

	/*std::vector<std::string> faces
	{
		"res/textures/envmap_violentdays/violentdays_rt.tga",
		"res/textures/envmap_violentdays/violentdays_lf.tga",
		"res/textures/envmap_violentdays/violentdays_up.tga",
		"res/textures/envmap_violentdays/violentdays_dn.tga",
		"res/textures/envmap_violentdays/violentdays_bk.tga",
		"res/textures/envmap_violentdays/violentdays_ft.tga"
	};*/

	std::vector<std::string> faces
	{
		"res/textures/purplevalley/purplevalley_rt.tga",
		"res/textures/purplevalley/purplevalley_lf.tga",
		"res/textures/purplevalley/purplevalley_up.tga",
		"res/textures/purplevalley/purplevalley_dn.tga",
		"res/textures/purplevalley/purplevalley_bk.tga",
		"res/textures/purplevalley/purplevalley_ft.tga"
	};
	Cubemap cubeMap(faces);



	//mvp

	glm::mat4 view = glm::mat4(1.0f);
	view = cam.GetViewMatrix();
	//model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//projection
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(cam.Zoom, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);

	glm::mat4 mvp = glm::mat4(1.0f);
	mvp = projection * view * model;




	float timeValue;
	float offsetValue;

	glm::vec3 cubePositions[10]{
		glm::vec3(0.0f,10.0f,0.0f),
		glm::vec3(1.0f,12.0f,-3.0f),
		glm::vec3(-2.0f,14.0f,-6.0f),
		glm::vec3(5.0f,14.6f,-9.0f),
		glm::vec3(3.0f,15.0f,-12.0f),
		glm::vec3(6.0f,17.0f,0.0f),
		glm::vec3(3.0f,16.0f,-7.70f),
		glm::vec3(-5.0f,14.0f,-6.0f),
		glm::vec3(7.0f,10.6f,-9.0f),
		glm::vec3(-3.0f,10.0f,-15.0f)
	};


	std::vector<glm::vec3> pointLightPositions = {
		glm::vec3(8.7f,  3.52f,  2.0f),
		glm::vec3(12.3f, 3.3f, 4.0f),
		glm::vec3(-4.0f,  3.0f, -2.0f),
		glm::vec3(0.0f,  4.0f, 3.0f)
	};


	DirectionalLight dirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.05f, 0.08f,0.161f), glm::vec3(0.6f, 0.58f, 0.97f), glm::vec3(0.51f, 0.31f, 0.801f));
	attenuation att = { 1.0f, 0.7f, 1.8f };

	//create pointlights
	for (unsigned int i = 0; i < pointLightPositions.size(); i++)
	{
		pointLights.push_back(PointLight(pointLightPositions[i], glm::vec3(0.01, 0.01, 0.01), glm::vec3(0.5, 0.4, 1.0), glm::vec3(0.5, 0.4, 1.0), 1.65f, att));

	}

	//Imgui debug values
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec4 lightposition = ImVec4(0.571f, 10.286f, 2.60f, 1.00f);
	ImVec4 lightambient = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);
	ImVec4 lightdiffuse = ImVec4(3.429f, 2.0f, 3.429f, 1.00f);
	ImVec4 lightspecular = ImVec4(3.429f, 2.0f, 3.429f, 1.00f);
	ImVec4 lightattenuation = ImVec4(1.0, 0.22f, 0.20f, 1.00f);
	float lightintensity = 1.0f;
	ImVec4 testposition = ImVec4(0.45f, 2.0f, 0.60f, 1.00f);
	ImVec4 framebufferPosition = ImVec4(0.0f,0.0f,0.0f,0.0f);
	ImVec4 framebufferViewPosition = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	cam.Position = glm::vec3(0.0, 9.0, 10.0);


	//Framebuffer
	//Frame buffer set up
	unsigned int fbo, framebufferTexture;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &framebufferTexture);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

	//render buffer object
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "error with framebuffer" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	cubeMap.Bind(reflectionMapShader);
	
	//GameObject stuff
	//Object testobject("testObject");
	/*testobject.addComponent(new DebugEngineComponent());
	testobject.addComponent(new Transform());
	Mesh testMesh = nano.meshes[0];
	testobject.addComponent(new Mesh(testMesh.vertices, testMesh.indices, testMesh.textures));
	*/

	/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{

			

			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			processInput(window);
			offsetValue = (sin(currentFrame) / 2.0f);

			//Refresh ui window
			ImGui_ImplGlfwGL3_NewFrame();

			// Perform a time step of the world in this frame.
			world->Step(deltaTime, velocityIterations, positionIterations);
			
			//Framebuffer first pass
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // | GL_STENCIL_BUFFER_BIT included
			glEnable(GL_DEPTH_TEST);
			
		
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			lightShader.use();
			lightShader.setInt("NUMBER_OF_TEXTURES", 1);
			lightShader.setInt("NOPL", 4);
			lightShader.setInt("NOSL", 0);
			lightShader.setVec3("viewPosition", cam.Position);
			lightShader.setFloat("mat.m_Shininess", 64.0f);

			reflectionMapShader.use();
			reflectionMapShader.setInt("NUMBER_OF_TEXTURES", 1);
			reflectionMapShader.setInt("NOPL", 4);
			reflectionMapShader.setInt("NOSL", 0);
			reflectionMapShader.setFloat("mat.m_Shininess", 64.0f);
			cubeMap.Bind(reflectionMapShader);

			view = cam.GetViewMatrix();
			lightShader.use();
			dirLight.Bind(lightShader);

			reflectionMapShader.use();
			dirLight.Bind(reflectionMapShader);

			
			for (unsigned int i = 0; i < pointLights.size(); i++)
			{
				reflectionMapShader.use();
				pointLights[i].Bind(reflectionMapShader, i);
				lightShader.use();
				pointLights[i].Bind(lightShader, i);

			}

			glStencilMask(0x00);
			lightShader.use();

			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(0.0f, 3.5f, 5.0f));
			model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
			mvp = projection * view * model;
			lightShader.setMat4("model", model);
			lightShader.setMat4("view", view);
			lightShader.setMat4("projection", projection);
			terrain.Draw(lightShader);

			b3Vec3 position = body->GetPosition();
			b3Quat orientation = body->GetOrientation();
			b3Vec3 axis;
			float32 angle;
			orientation.GetAxisAngle(&axis, &angle);

			


			//reflection test
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(2.0, 6.0, 5.0));
			reflectionShader.use();
			reflectionShader.setMat4("model", model);
			reflectionShader.setMat4("view", view);
			reflectionShader.setMat4("projection", projection);
			reflectionShader.setVec3("cameraPosition", cam.Position);
			reflectiveCube.Draw(reflectionShader, cubeMap.textureId);


			//reflection map test
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(lightposition.x, lightposition.y, lightposition.z));
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			reflectionMapShader.use();
			reflectionMapShader.setMat4("model", model);
			reflectionMapShader.setMat4("view", view);
			reflectionMapShader.setMat4("projection", projection);
			reflectionMapShader.setVec3("viewPosition", cam.Position);
			nano.Draw(reflectionMapShader);

			//grass shader relied on deleted default.vert, update this soon
			grassShader.use();
			grassShader.setMat4("view", view);
			grassShader.setMat4("projection", projection);\

				for (unsigned int i = 0; i < 10; i++)
				{
					model = glm::mat4(1.0);
					model = glm::translate(model, cubePositions[i]);
					model = glm::scale(model, glm::vec3(3, 3, 3));
					grassShader.setMat4("model", model);
					quad.Draw(grassShader);

				}


			//Stencil
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
			model = glm::rotate(model, angle, glm::vec3(axis.x, axis.y, axis.z));
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			reflectionMapShader.use();
			reflectionMapShader.setMat4("model", model);
			reflectionMapShader.setMat4("view", view);
			reflectionMapShader.setMat4("projection", projection);

			nano.Draw(reflectionMapShader);

			glStencilFunc(GL_LESS, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);
			outlineShader.use();
			model = glm::scale(model, glm::vec3(1.03f, 1.03f, 1.03f));
			outlineShader.setMat4("view", view);
			outlineShader.setMat4("projection", projection);
			outlineShader.setMat4("model", model);

			nano.Draw(outlineShader);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
			glEnable(GL_DEPTH_TEST);
		

			cubeMap.Draw(cubemapShader, view, projection);

			//Framebuffer time
			// second pass
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
			glDisable(GL_DEPTH_TEST);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			fbShader.use();
			renderQuad.Draw(fbShader, "screenTexture", framebufferTexture);




			//ui stuff
			// 1. Show a simple window.
			// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
			//New menus can be created this way.
			{
				if (ImGui::Begin("STUFF")) {

					if (ImGui::BeginMenu("Debug Stuff"))
					{
						//std::cout << "menu working, fps wasted" << std::endl;

						static float f = 0.0f;
						static int counter = 0;
						ImGui::Text("Debug Menu");
	
						// Display some text (you can use a format string too)        // Edit 1 float using a slider from 0.0f to 1.0f    
						ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
						if (ImGui::BeginMenu("Point Light"))
						{
							ImGui::SliderFloat3("light pos", &lightposition.x, -40, 40);
							ImGui::SliderFloat("Light intensity", &lightintensity, 0, 25);
							ImGui::ColorEdit3("light ambient", (float*)&lightambient);
							ImGui::ColorEdit3("light diffuse", (float*)&lightdiffuse);
							ImGui::ColorEdit3("light specular", (float*)&lightspecular);
							ImGui::DragFloat3("light attenuation", &lightattenuation.x, -2, 2);
							ImGui::EndMenu();
						}


						ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

						if (ImGui::Button("addForce"))
						{
							body->ApplyForce(b3Vec3(10000, 10000.0f, 0), b3Vec3(1, 0, 0), true);
						}

						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("GameObjects"))
					{
						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Lights"))
					{
						if (ImGui::BeginMenu("Point Lights"))
						{
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu("Spot Lights"))
						{
							ImGui::EndMenu();
						}
						ImGui::EndMenu();
					}

					if (ImGui::BeginMenu("Sound"))
					{
						if (ImGui::BeginMenu("Pure Data"))
						{
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu("Sample Based"))
						{
							ImGui::EndMenu();
						}
						ImGui::EndMenu();
					}

				}
				ImGui::End();

			}

			//ui render
			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());


			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			/* Poll for and process events */
			glfwPollEvents();
		}

	// Cleanup
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	glDeleteFramebuffers(1, &fbo);

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
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
			cam.canMove = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		cam.canMove = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
