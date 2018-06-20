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
	attenuation defaultAtt = { 1,0,0 };
	this->position = glm::vec3(0.0f);
	this->ambient = glm::vec3(0.0f);
	this->diffuse = glm::vec3(0.0f);
	this->specular = glm::vec3(0.0f);
	this->att = defaultAtt;
}

void PointLight::intitalize(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, attenuation att)
{
	this->position = position;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->att = att;
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
	shader.setFloat(s + "constant", att.constant);
	shader.setFloat(s + "linear",att.linear);
	shader.setFloat(s + "quadratic", att.quadratic);
}