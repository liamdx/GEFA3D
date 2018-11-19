#pragma once
#include "Examples\Example.h"
#include "Examples\EngineCore.h"
#include "Common.h"
#include "animation\AnimatedModel.h"

class AnimationTest : Example
{
public:
	AnimationTest(GLFWwindow* newWindow) : cyborg("res/models/monster/Monster_1.dae"), boneShader("res/shaders/Debug/BoneDebug.vert", "res/shaders/Debug/BoneDebug.frag"), window(newWindow), input(newWindow) {}
	~AnimationTest();
	void init() override;
	void start() override;
	void earlyUpdate(float deltaTime) override;
	void update(float deltaTime) override;
	void UI() override;

	AnimatedModel cyborg;
	Shader boneShader;
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