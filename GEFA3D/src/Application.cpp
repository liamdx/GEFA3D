#define _CRT_SECURE_NO_WARNINGS


#include "Common.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "filesystem.h"
//#include "Texture.h"
#include "lighting\DirectionalLight.h"
#include "lighting\PointLight.h"
#include "lighting\SpotLight.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw_gl3.h"



//Engine Time
float deltaTime = 0.0f;
float lastFrame = 0.0f;

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
	window = glfwCreateWindow(1600, 900, "GEFA3D", NULL, NULL);
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
	ImGui::StyleColorsLight();
	

	//Mouse input handle

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(window, mouse_callback);


	//Enable depth
	glEnable(GL_DEPTH_TEST);

	Shader lightShader("res/light.vert", "res/light.frag");
	Shader lampShader("res/lamp.vert", "res/lamp.frag");

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	//cube VAO
	unsigned int vao;
	unsigned int vbo;
	glGenVertexArrays(1, &vao);
	//Bind the buffers and vertex attribute(s), vao
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(vao);
	//Create a vertex attribute (layout = 0 in shader)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//normal attribute 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//texture coords attribute 
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//texture attribute 
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	
	//light vao
	unsigned int lightVao;
	glGenVertexArrays(1, &lightVao);
	glBindVertexArray(lightVao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//Create a vertex attribute (layout = 0 in shader)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	


	//mvp

	glm::mat4 view = glm::mat4(1.0f);
	view = cam.GetViewMatrix();
	//model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//projection
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(cam.Zoom, 1600.0f / 900.0f , 0.1f, 100.0f);
	
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
	DirectionalLight dirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.01f, 0.01f, 0.01f), glm::vec3(0.01f, 0.01f, 0.01f), glm::vec3(0.01f, 0.01f, 0.01f));
	attenuation att = { 1.0f, 0.22f, 0.2f };
	PointLight pl = PointLight(glm::vec3(1.0), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.25f, 1.25f), glm::vec3(1.0f, 1.0f, 1.0f), att);

	std::string filepath = __FILE__;
	for (int i = 0; i < 15; i++)
	{
		filepath.pop_back();
	}
	std::cout << filepath << std::endl;


	static std::string nanoPath = "C:/Users/Liam/source/repos/GEFA3D/GEFA3D/res/nanosuit/nanosuit.obj";
	Model nano(nanoPath);
	static std::string terrainPath = "C:/Users/Liam/source/repos/GEFA3D/GEFA3D/res/sponza_obj/sponza.obj";
	Model terrain(terrainPath);

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec4 lightposition = ImVec4(0.571f, 0.286f, 2.60f, 1.00f);
	ImVec4 lightambient = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);
	ImVec4 lightdiffuse = ImVec4(3.429f, 2.0f, 3.429f, 1.00f);
	ImVec4 lightspecular = ImVec4(3.429f, 2.0f, 3.429f, 1.00f);
	ImVec4 lightattenuation = ImVec4(1.0, 0.22f, 0.20f, 1.00f);
	ImVec4 nanoposition = ImVec4(0.45f, -3.429f, 0.60f, 1.00f);

	for (int i = 0; i < nano.textures_loaded.size(); i++)
	{
		std::cout << nano.textures_loaded[i].t_Id << std::endl;
		std::cout << nano.textures_loaded[i].t_Path << std::endl;
		std::cout << nano.textures_loaded[i].t_Type << std::endl;
	}
	for (int i = 0; i < terrain.textures_loaded.size(); i++)
	{
		std::cout << terrain.textures_loaded[i].t_Id << std::endl;
		std::cout << terrain.textures_loaded[i].t_Path << std::endl;
		std::cout << terrain.textures_loaded[i].t_Type << std::endl;
	}
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		processInput(window); 
		timeValue = glfwGetTime();
		offsetValue = (sin(timeValue) / 2.0f);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		ImGui_ImplGlfwGL3_NewFrame();

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
		//lightShader.setInt("mat.m_Diffuse[0]", diffuseTexture.slot);
		lightShader.setInt("NOPL", 1);
		lightShader.setInt("NOSL", 0);
		//lightShader.setInt("mat.m_Specular[0]", specularTexture.slot);
		lightShader.setVec3("viewPosition", cam.Position);
		lightShader.setFloat("mat.m_Shininess", 32.0f);

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

		pl.Bind(lightShader);
		
		
		
		lightShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(nanoposition.x, nanoposition.y, nanoposition.z));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		mvp = projection * view * model;
		lightShader.setMat4("model", model);
		lightShader.setMat4("mvp", mvp);
		nano.Draw(lightShader);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -3.75f, 5.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		mvp = projection * view * model;
		lightShader.setMat4("model", model);
		lightShader.setMat4("mvp", mvp);
		terrain.Draw(lightShader);

		lampShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(lightposition.x, lightposition.y, lightposition.z));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		mvp = projection * view * model;
		lampShader.setMat4("mvp", mvp);
		lampShader.setVec3("color", pl.diffuse);
		glBindVertexArray(lightVao);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//ui stuff
		// 1. Show a simple window.
		// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
		{
			static float f = 0.0f;
			static int counter = 0;
			ImGui::Text("Debug");                           // Display some text (you can use a format string too)        // Edit 1 float using a slider from 0.0f to 1.0f    
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
			ImGui::SliderFloat3("suit pos", &nanoposition.x, -10, 10);
			ImGui::SliderFloat3("light pos", &lightposition.x, -10, 10);
			ImGui::SliderFloat3("light ambient", &lightambient.x, -10, 10);
			ImGui::SliderFloat3("light diffuse", &lightdiffuse.x, -10, 10);
			ImGui::SliderFloat3("light specular", &lightspecular.x, -10, 10);
			ImGui::InputFloat3("light attenuation", &lightattenuation.x, -2, 2);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
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

float clip(float n, float lower, float upper) {
	return std::max(lower, std::min(n, upper));
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
