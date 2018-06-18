#include "Common.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

std::string VertexFilepath = "res/default.vert";
std::string FragmentFilepath = "res/default.frag";

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

glm::vec3 lightPosition(1.2f, 1.0f, 2.0f);

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
	window = glfwCreateWindow(1024, 600, "GEFA3D", NULL, NULL);
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

	//Mouse input handle

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
	
	



	glm::mat4 view = glm::mat4(1.0f);
	view = cam.GetViewMatrix();
	//mvp
	//model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//projection
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(cam.Zoom, 1024.0f / 600.0f , 0.1f, 100.0f);
	glm::mat4 mvp = glm::mat4(1.0f);
	mvp = projection * view * model;


	Texture diffuseTexture("res/container2.png",0);
	Texture specularTexture("res/container2_specular.png", 1);

	std::cout << diffuseTexture.t_Slot << std::endl;
	std::cout << specularTexture.t_Slot << std::endl;

	
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


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		timeValue = glfwGetTime();
		offsetValue = (sin(timeValue) / 2.0f);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		/* Render here */
		//glClearColor(0.678f, 0.847f, 0.901f, 1.0f);
		//glClearColor(0.0f, 0.749f, 0.647f, 1.0f);
		glClearColor(0.03f, 0.02f, 0.01f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightShader.use();
		lightShader.setInt("mat.m_Diffuse", diffuseTexture.slot);
		lightShader.setInt("mat.m_Specular", specularTexture.slot);
		lightShader.setVec3("viewPosition", cam.Position);
		lightShader.setVec3("mat.m_Ambient", glm::vec3(0.135f,0.2225f,0.1575f));
		lightShader.setFloat("mat.m_Shininess", 1.0f);
		lightShader.setVec3("light.l_Ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		lightShader.setVec3("light.l_Diffuse", glm::vec3(0.6f, 0.6f, 0.6f)); // darken the light a bit to fit the scene
		lightShader.setVec3("light.l_Specular", glm::vec3(1.0f, 1.0f, 1.0f));
		lightShader.setVec3("light.position", cam.Position);
		lightShader.setVec3("light.direction", cam.Front);
		lightShader.setFloat("light.innerCutoff", glm::cos(glm::radians(15.0f)));
		lightShader.setFloat("light.outerCutoff", glm::cos(glm::radians(20.0f)));
		lightShader.setFloat("light.constant", 1.0f);
		lightShader.setFloat("light.linear", 0.09f);
		lightShader.setFloat("light.quadratic", 0.032f);
		
		view = cam.GetViewMatrix();

		for (unsigned int i = 0; i < 10; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20 * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			mvp = projection * view * model;
			lightShader.setVec3("light.position", cam.Position);
			lightShader.setVec3("light.direction", cam.Front);
			lightShader.setFloat("light.innerCutoff", glm::cos(glm::radians(12.5f)));
			lightShader.setFloat("light.outerCutoff", glm::cos(glm::radians(17.5f)));

			lightShader.setMat4("mvp", mvp);
			lightShader.setMat4("model", model);

			diffuseTexture.Use();
			specularTexture.Use();

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		
		lampShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.2f, 1.0f, 3.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		mvp = projection * view * model;
		lampShader.setMat4("mvp", mvp);
		
		glBindVertexArray(lightVao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	

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
