
#include "NormalTestExample.h"

//namespace example {

float cRadius = 5.0f;

OrbitCamera testCam(glm::vec3(1.0, 6.0, -2.0));

glm::vec3 charLocation = glm::vec3(0, 4.0, 2);

float speed = 5.0f;

NormalTest::~NormalTest()
{

}

void NormalTest::init() 
{

}

void NormalTest::start()
{
	projection = glm::perspective(90.0f, (float)1280 / 720, 0.1f, 100.0f);	
	testCam.MovementSpeed = 1.0f;
}

void NormalTest::earlyUpdate(float deltaTime)
{
	testCam.heightOffset = 2.0f;

	if (input.GetKeyW())
	{
		charLocation += glm::vec3(0, 0, 1) *deltaTime * speed;
	}

	if (input.GetKeyS())
	{
		charLocation -= glm::vec3(0, 0, 1) * deltaTime * speed;
	}

	if (input.GetKeyA())
	{
		charLocation += glm::vec3(1, 0, 0) * deltaTime * speed;
	}

	if (input.GetKeyD())
	{
		charLocation -= glm::vec3(1, 0, 0) * deltaTime * speed;
	}

	if (input.GetRightClick())
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		testCam.canMove = true;
		input.GetMouseMovement();

		float deltaX = input.xpos - lastX;
		float deltaY = input.ypos - lastY;

		testCam.ProcessMouseMovement(input.xpos, -input.ypos, deltaTime);

	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		testCam.canMove = false;
	}

	testCam.Target = charLocation;
	testCam.radius = cRadius;
	view = testCam.GetViewMatrix();

}

void NormalTest::update(float deltaTime)
{

	
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glm::vec3 currentPointLightPosition = glm::vec3(pointLightPosition.x, pointLightPosition.y, pointLightPosition.z);
	

	normalShader.use();
	normalShader.setMat4("projection", projection);
	normalShader.setMat4("view", view);
	normalShader.setVec3("viewPosition", testCam.Position);
	normalShader.setVec3("pointLightPosition", currentPointLightPosition);

	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 3.5f, 0.0f));
	model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
	normalShader.setMat4("model", model);
	terrain.Draw(normalShader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, charLocation);
	normalShader.setMat4("model", model);
	cyborg.Draw(normalShader);

	

	lastX = input.xpos;
	lastY = input.ypos;
}


void NormalTest::UI()
{
	if (ImGui::Begin("NormalTest Example")) {

		if (ImGui::BeginMenu("Example Options"))
		{
			ImGui::DragFloat3("Point Light Position: ", &pointLightPosition.x,0.1f,  -40, 40);
			ImGui::DragFloat3("Cyborg Position: ", &cyborgLocation.x,0.1f, -40, 40);
			ImGui::DragFloat3("Camera Position: ", &cameraLocation.x,0.1f, -40, 40);
			ImGui::SliderFloat("Camera Radius", &cRadius, 2.0, 15.0);
			ImGui::EndMenu();
		}

		ImGui::End();
	}
}
//}