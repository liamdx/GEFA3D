#pragma once

#include "Common.h"
#include "Shader.h"

class SpotLight
{
public:
	SpotLight(glm::vec3 position, glm::vec3 direction,float innerCutoff, float outerCutoff, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, attenuation att);
	SpotLight();
	void initialize(glm::vec3 position, glm::vec3 direction, float innerCutoff, float outerCutoff, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, attenuation att);
	void Bind(Shader shader, unsigned int index);

	glm::vec3 position;
	glm::vec3 direction;
	float innerCutoff;
	float outerCutoff;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	attenuation att;

};