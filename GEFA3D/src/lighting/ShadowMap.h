#pragma once

#include "Common.h"
#include "Shader.h"
#include "lighting\DirectionalLight.h"
class ShadowMap
{
public:

	ShadowMap() : shadowMapDepthShader("res/shaders/Shadowmap.vert", "res/shaders/Shadowmap.frag") {};
	~ShadowMap();

	void init();
	void update(glm::vec3 camPosition, glm::mat4 viewMatrix, DirectionalLight dirLight);

	unsigned int depthMap, depthMapFBO;
	Shader shadowMapDepthShader;
	unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	glm::mat4 lightSpaceMatrix = glm::mat4(1.0);
	float near_plane = 0.1f, far_plane = 100.0f;
};