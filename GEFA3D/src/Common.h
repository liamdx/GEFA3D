#pragma once

// Program scope
struct attenuation
{
	float constant;
	float linear;
	float quadratic;
};

#include "EngineComponent.h"

//External
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "ext/glm/glm.hpp"
#include "ext/glm/gtc/matrix_transform.hpp"
#include "ext/glm/gtc/type_ptr.hpp"

#include <string>

#include <algorithm>

#include <fstream>

#include <sstream>

#include <vector>

#include <iostream> 

#include <memory>

//Image loader (STLI should work too)
#include "ext\stb_image\stb_image.h"

//Physics integration
#include "bounce\bounce.h"
