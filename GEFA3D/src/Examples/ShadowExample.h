#pragma once

#include "Examples\Example.h"
#include "Examples\EngineCore.h"


class ShadowTest : Example
{
public:
	ShadowTest(GLFWwindow* newWindow) : cyborg("res/models/cyborg/cyborg.obj"), terrain("res/models/sponza_obj/sponza.obj"), shadowMapDepthShader("res/shaders/Shadowmap.vert", "res/shaders/Shadowmap.frag"), cubemapShadowDepthShader("res/shaders/CubeDepthMap.vert", "res/shaders/CubeDepthMap.geo", "res/shaders/CubeDepthMap.frag"), shadowShader("res/shaders/Shadow.vert", "res/shaders/Shadow.frag"), window(newWindow), input(newWindow) {} 
	~ShadowTest() {} ;
	void init() override;
	void start() override;
	void earlyUpdate(float deltaTime) override;
	void update(float deltaTime) override;
	void UI() override;

	Model terrain;
	Model cyborg;

	Shader shadowMapDepthShader;
	Shader cubemapShadowDepthShader;
	Shader shadowShader;

	GLFWwindow* window;

	InputManager input;

	unsigned int depthMap, depthMapFBO;

	unsigned int depthCubemap, depthCubemapFBO; // Single point light shadow map	

	glm::vec3 pointLightPosition; //Debug pointlight position
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	float near_plane = 0.1f, far_plane = 100.0f;
	float OrbitSpeed = 0.35f;
	float lastX, lastY;

	ImVec4 lightposition = ImVec4(-2.0f, 4.0f, -1.0f, 1.00f);
	ImVec4 lightcenter = ImVec4(0.0f, 3.5f, 0.0f, 1.00f);
	ImVec4 position = ImVec4(4.0, 3.5, -2.0, 1.0);
	ImVec4 plposition = ImVec4(1.0, 6.0, -2.0, 1.0);
	ImVec4 cameraLocation = ImVec4(1.0, 6.0, -2.0, 1.0);

	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 view = glm::mat4(1.0);
	glm::mat4 projection = glm::mat4(1.0);
	glm::mat4 lightSpaceMatrix = glm::mat4(1.0) ;
};