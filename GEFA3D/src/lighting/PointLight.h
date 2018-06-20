#pragma once

#include "Common.h"
#include "Shader.h"



class PointLight 
{
public:
	PointLight(glm::vec3 position, glm::vec3 ambient,glm::vec3 diffuse,glm::vec3 specular, attenuation att);
	PointLight();
	void intitalize(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, attenuation att);
	void Bind(Shader shader, unsigned int index);
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	attenuation att;
private:

};