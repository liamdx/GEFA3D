#pragma once

#include "Common.h"
#include "Shader.h"

class SpotLight
{
public:
	SpotLight(glm::vec3 position, glm::vec3 direction,float innerCutoff, float outerCutoff, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float intensity, attenuation att);
	SpotLight();
	void initialize(glm::vec3 position, glm::vec3 direction, float innerCutoff, float outerCutoff, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,float intensity, attenuation att);
	void Bind(Shader shader, unsigned int index);

	glm::vec3 position;
	glm::vec3 direction;
	float innerCutoff;
	float outerCutoff;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float intensity;
	attenuation att;

	inline void setPosition(glm::vec3 newPosition) { position = newPosition; }
	inline void setDirection(glm::vec3 newDirection) { direction = newDirection; }
	inline void setAmbient(glm::vec3 newAmbient) { ambient = newAmbient; }
	inline void setDiffuse(glm::vec3 newDiffuse) { diffuse = newDiffuse; }
	inline void setSpecular(glm::vec3 newSpecular) { specular = newSpecular; }
	inline void setAttenuation(glm::vec3 newAttenuation) { att.constant = newAttenuation.x, att.linear = newAttenuation.y, att.quadratic = newAttenuation.z; }
	inline void setIntensity(float newIntensity) { intensity = newIntensity; }
	inline void setInnerCutoff(float newInnerCutoff) { innerCutoff = newInnerCutoff; }
	inline void setOuterCutoff(float newOuterCutoff) { outerCutoff = newOuterCutoff; }

};