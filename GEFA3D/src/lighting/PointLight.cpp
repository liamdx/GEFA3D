#include "PointLight.h"

PointLight::PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, attenuation att)
{
	this->position = position;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->att = att;
}

PointLight::PointLight()
{
	attenuation defaultAtt = { 0,0,0 };
	this->position = glm::vec3(0.0f);
	this->ambient = glm::vec3(0.0f);
	this->diffuse = glm::vec3(0.0f);
	this->specular = glm::vec3(0.0f);
	this->att = defaultAtt;
}