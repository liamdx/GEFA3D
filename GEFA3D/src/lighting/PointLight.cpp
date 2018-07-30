#include "PointLight.h"

PointLight::PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float intensity, float distance)
{
	this->position = position;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->intensity = intensity;
	this->distance = distance;
}

PointLight::PointLight()
{
	this->position = glm::vec3(0.0f);
	this->ambient = glm::vec3(0.0f);
	this->diffuse = glm::vec3(0.0f);
	this->specular = glm::vec3(0.0f);
	this->intensity = 0.0f;
	this->distance = 1.0f;
}

void PointLight::intitalize(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,float intensity,  float distance)
{
	this->position = position;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->intensity = intensity;
	this->distance = distance;
}

void PointLight::Bind(Shader shader, unsigned int index)
{
	std::stringstream ss;
	ss << "pointLights[" << index << "].";
	std::string s = ss.str();

	shader.setVec3(s + "position", position);
	shader.setVec3(s + "ambient", ambient);
	shader.setVec3(s + "diffuse", diffuse);
	shader.setVec3(s + "specular", specular);
	shader.setFloat(s + "intensity", intensity);
	shader.setFloat(s + "distance", distance);
}

void PointLight::Bind(Shader shader)
{
	std::stringstream ss;
	ss << "debugLight.";
	std::string s = ss.str();

	shader.setVec3(s + "position", position);
	shader.setVec3(s + "ambient", ambient);
	shader.setVec3(s + "diffuse", diffuse);
	shader.setVec3(s + "specular", specular);
	shader.setFloat(s + "intensity", intensity);
	shader.setFloat(s + "distance", distance);

}