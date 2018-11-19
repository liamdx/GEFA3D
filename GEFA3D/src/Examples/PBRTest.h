#pragma once
#include "Examples\Example.h"
#include "Examples\EngineCore.h"


class PBRTest : Example
{
public:
	PBRTest(GLFWwindow* newWindow, std::vector<std::string> faces) : cyborg("res/models/xwing/xwing.obj"), PBRShader("res/shaders/PBR.vert", "res/shaders/PBR.frag"), skyboxShader("res/shaders/Cubemap.vert", "res/shaders/Cubemap.frag"), window(newWindow), input(newWindow), skybox(faces) {}
	~PBRTest();
	void init() override;
	void start() override;
	void earlyUpdate(float deltaTime) override;
	void update(float deltaTime) override;
	void UI() override;

	
	Cube testCube;
	Model cyborg;
	Shader PBRShader;
	Shader skyboxShader;
	InputManager input;
	Cubemap skybox;

	ImVec4 pointLightPosition = ImVec4(2.0, 5.0, 3.0, 1.0);
	ImVec4 cyborgLocation = ImVec4(4.0, 3.5, -2.0, 1.0);


	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 view = glm::mat4(1.0);
	glm::mat4 projection = glm::mat4(1.0);

	GLFWwindow* window;

	unsigned int albedo, normal, metallic, roughness, ao;

private:

	float lastX, lastY;
	

};