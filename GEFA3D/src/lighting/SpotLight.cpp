#include "SpotLight.h"

SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, float innerCutoff, float outerCutoff, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float intensity, attenuation att)
{
	this->position = position;
	this->direction = direction;
	this->innerCutoff = innerCutoff;
	this->outerCutoff = outerCutoff;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->intensity = intensity;
	this->att = att;
}

SpotLight::SpotLight()
{
	attenuation defaultAtt = { 1,0,0 };
	this->position = glm::vec3(0.0f);
	this->direction = glm::vec3(0.0f);;
	this->innerCutoff = 0.0f;
	this->outerCutoff = 0.0f;
	this->ambient = glm::vec3(0.0f);
	this->diffuse = glm::vec3(0.0f);
	this->specular = glm::vec3(0.0f);
	this->att = defaultAtt;
}
void SpotLight::initialize(glm::vec3 position, glm::vec3 direction, float innerCutoff, float outerCutoff, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float intensity, attenuation att)
{
	this->position = position;
	this->direction = direction;
	this->innerCutoff = innerCutoff;
	this->outerCutoff = outerCutoff;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->intensity = intensity;
	this->att = att;
}

void SpotLight::Bind(Shader shader, unsigned int index)
{
	std::stringstream ss;
	ss << "spotLights[" << index << "].";
	std::string s = ss.str();

	shader.setVec3(s + "position", position);
	shader.setVec3(s + "direction", direction);
	shader.setVec3(s + "ambient", ambient);
	shader.setVec3(s + "diffuse", diffuse);
	shader.setVec3(s + "specular", specular);
	shader.setFloat(s + "innerCutoff", innerCutoff);
	shader.setFloat(s + "outerCutoff", outerCutoff);
	shader.setFloat(s + "intensity", intensity);
	shader.setFloat(s + "constant", att.constant);
	shader.setFloat(s + "linear", att.linear);
	shader.setFloat(s + "quadratic", att.quadratic);
}