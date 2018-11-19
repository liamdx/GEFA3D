
#include "ReflectionExample.h"

//namespace example {



Camera RCam(glm::vec3(1.0, 6.0, -2.0));


ReflectionTest::ReflectionTest(GLFWwindow* newWindow, std::vector<std::string> faces)
	: cyborg("res/models/nanosuit_reflection/nanosuit.obj"), terrain("res/models/sponza_obj/sponza.obj"), reflectionShader("res/shaders/DefaultReflective.vert", "res/shaders/DefaultReflective.frag"), skyboxShader("res/shaders/Cubemap.vert", "res/shaders/Cubemap.frag"), window(newWindow), cubeMap(faces)
{
	cubeMap.Bind(reflectionShader);
}
ReflectionTest::~ReflectionTest()
{

}

void ReflectionTest::init()
{

}

void ReflectionTest::start()
{
	projection = glm::perspective(90.0f, (float)1280 / 720, 0.1f, 100.0f);
}

void ReflectionTest::earlyUpdate(float deltaTime)
{


}

void ReflectionTest::update(float deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::vec3 currentCameraLocation = glm::vec3(cameraLocation.x, cameraLocation.y, cameraLocation.z);
	RCam.Position = currentCameraLocation;
	view = RCam.GetViewMatrix();
	glm::vec3 currentPointLightPosition = glm::vec3(pointLightPosition.x, pointLightPosition.y, pointLightPosition.z);
	reflectionShader.use();
	reflectionShader.setMat4("projection", projection);
	reflectionShader.setMat4("view", view);
	reflectionShader.setVec3("viewPosition", RCam.Position);


	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 3.5f, 0.0f));
	model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
	reflectionShader.setMat4("model", model);
	terrain.Draw(reflectionShader);

	glm::vec3 currentCyborgLocation = glm::vec3(cyborgLocation.x, cyborgLocation.y, cyborgLocation.z);
	model = glm::mat4(1.0f);
	model = glm::translate(model, currentCyborgLocation);
	reflectionShader.setMat4("model", model);
	cyborg.Draw(reflectionShader);

}


void ReflectionTest::UI()
{
	if (ImGui::Begin("ReflectionTest Example")) {

		if (ImGui::BeginMenu("Example Options"))
		{
			ImGui::DragFloat3("Point Light Position: ", &pointLightPosition.x, 0.1f, -40, 40);
			ImGui::DragFloat3("Cyborg Position: ", &cyborgLocation.x, 0.1f, -40, 40);
			ImGui::DragFloat3("Camera Position: ", &cameraLocation.x, 0.1f, -40, 40);
			ImGui::EndMenu();
		}

		ImGui::End();
	}
}
//}