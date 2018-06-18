#pragma once

#include "Common.h"

struct attenuation
{
	float constant;
	float linear;
	float quadratic;
};


class PointLight 
{
public:
	PointLight(glm::vec3 position, glm::vec3 ambient,glm::vec3 diffuse,glm::vec3 specular, attenuation att);
	PointLight();
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	attenuation att;

};