#pragma once
#include "Examples\Example.h"
#include "Examples\EngineCore.h"
#include "Common.h"
#include "animation\AnimatedModel.h"
#include "ext\imgui\ImGuiFileDialog.h"

class AnimationTest : Example
{
public:
	AnimationTest(GLFWwindow* newWindow, std::vector<std::string> skyboxFaces)
		: boneShader("res/shaders/Debug/BoneDebug.vert", "res/shaders/Debug/BoneDebug.frag"),
		window(newWindow), input(newWindow), skyboxShader("res/shaders/Cubemap.vert", "res/shaders/Cubemap.frag"), mcSkybox(skyboxFaces), level("res/models/sponza_obj/sponza.obj"), reflectionShader("res/shaders/Reflective.vert", "res/shaders/Reflective.frag")
	 {}


	~AnimationTest();
	void init() override;
	void start() override;
	void earlyUpdate(float deltaTime) override;
	void update(float deltaTime) override;
	void UI() override;

	void SetBoneTransform(Shader shader, unsigned int index, glm::mat4 transform)
	{
		std::stringstream currentBoneLocation;
		currentBoneLocation << "gBones[" << index << "]";
		shader.setMat4(currentBoneLocation.str(), transform);
		currentBoneLocation.clear();
	}

	void SetBoneTransformId(Shader shader, unsigned int id, glm::mat4 transform)
	{
		shader.setMat4ID(id, transform);
	}

	Shader boneShader;
	Shader reflectionShader;
	Shader skyboxShader;
	InputManager input;
	AnimatedModel anim;
	Model level;

	Cubemap mcSkybox;

	ImVec4 pointLightPosition = ImVec4(2.0, 5.0, 3.0, 1.0);
	ImVec4 cyborgLocation = ImVec4(4.0, 3.5, -2.0, 1.0);
	ImVec4 cameraLocation = ImVec4(1.0, 6.0, -2.0, 1.0);


	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 view = glm::mat4(1.0);
	glm::mat4 projection = glm::mat4(1.0);
	std::vector<glm::mat4> transforms;

	GLFWwindow* window;

private:
	static const int MAX_NUM_BONES = 250;
	unsigned int m_boneLocation[MAX_NUM_BONES];
	unsigned int textureId;
	float lastX, lastY;

	std::vector<unsigned int> gBoneLocations;

	float runningTime;


};