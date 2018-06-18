#pragma once
#include "Common.h"

class Light {
public:
	unsigned int id;

	Light();
	~Light();

private:
	//getter setter
	glm::vec3 position;
};