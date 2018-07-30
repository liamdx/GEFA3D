#pragma once

#include "Common.h"
#include "Shader.h"



class PointLight 
{
public:
	PointLight(glm::vec3 position, glm::vec3 ambient,glm::vec3 diffuse,glm::vec3 specular, float intensity, float distance);
	PointLight();
	void intitalize(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float intensity, float distance);
	void Bind(Shader shader, unsigned int index);
	void Bind(Shader shader);
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float intensity;
	float distance;
	inline void setPosition(glm::vec3 newPosition) { position = newPosition; }
	inline void setAmbient(glm::vec3 newAmbient) { ambient = newAmbient; }
	inline void setDiffuse(glm::vec3 newDiffuse) { diffuse = newDiffuse; }
	inline void setSpecular(glm::vec3 newSpecular) { specular = newSpecular; }
	inline void setDistance(float newDistance) { distance = newDistance; }
	inline void setIntensity(float newIntensity) { intensity = newIntensity; }
private:

};