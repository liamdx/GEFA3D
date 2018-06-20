#pragma once


struct attenuation
{
	float constant;
	float linear;
	float quadratic;
};


#include "GL\glew.h"

#include <GLFW/glfw3.h>

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"

#include <string>

#include <algorithm>

#include <fstream>

#include <sstream>

#include <vector>

#include <iostream>

#include "ext\stb_image\stb_image.h"
