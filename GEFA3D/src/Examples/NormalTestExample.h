#pragma once
#include "Examples\Example.h"
#include "Examples\EngineCore.h"


class NormalTest : Example
{
public:
	NormalTest(GLFWwindow* newWindow) : cyborg("res/models/cyborg/cyborg.obj"), terrain("res/models/sponza_obj/sponza.obj"), normalShader("res/shaders/NormalMap.vert", "res/shaders/NormalMap.frag"), window(newWindow), input(newWindow) {}
	~NormalTest();
	void init() override;
	void start() override;
	void earlyUpdate(float deltaTime) override;
	void update(float deltaTime) override;
	void UI() override;

	Model terrain;
	Model cyborg;
	Shader normalShader;
	InputManager input;

	ImVec4 pointLightPosition = ImVec4(2.0, 5.0, 3.0, 1.0);
	ImVec4 cyborgLocation = ImVec4(4.0, 3.5, -2.0, 1.0);
	ImVec4 cameraLocation = ImVec4(1.0, 6.0, -2.0, 1.0);


	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 view = glm::mat4(1.0);
	glm::mat4 projection = glm::mat4(1.0);

	GLFWwindow* window;

private:

	float lastX, lastY;


};