#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	this->direction = direction;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
}

DirectionalLight::DirectionalLight()
{
	this->direction = glm::vec3(0.0f);
	this->ambient = glm::vec3(0.0f);
	this->diffuse = glm::vec3(0.0f);
	this->specular = glm::vec3(0.0f);
}

void DirectionalLight::initialize(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	this->direction = direction;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
}

void DirectionalLight::Bind(Shader shader)
{
	std::string s = "dirLight.";

	shader.setVec3(s + "direction", direction);
	shader.setVec3(s + "ambient", ambient);
	shader.setVec3(s + "diffuse", diffuse);
	shader.setVec3(s + "specular", specular);
}
