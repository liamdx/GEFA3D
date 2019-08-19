#pragma once

#define GLM_ENABLE_EXPERIMENTAL

// Program scope
struct attenuation
{
	float constant;
	float linear;
	float quadratic;
};

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// Libs
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "ext/glm/glm.hpp"
#include "ext/glm/gtc/matrix_transform.hpp"
#include "ext/glm/gtc/type_ptr.hpp"
#include "ext/glm/gtx/quaternion.hpp"
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"
#include <string>
#include <algorithm>
#include <time.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream> 
#include <memory>
#include <map>
#include "ext\stb_image\stb_image.h"
#include "bounce\bounce.h"
#include "yse.hpp"

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }



#ifndef SYSOUT_F
#define SYSOUT_F(f, ...)      _RPT1( 0, f, __VA_ARGS__ ) // For Visual studio
#endif

#ifndef speedtest__             
#define speedtest__(data)   for (long blockTime = NULL; (blockTime == NULL ? (blockTime = clock()) != NULL : false); SYSOUT_F(data "%.9fs", (double) (clock() - blockTime) / CLOCKS_PER_SEC))
#endif

class common {
public:
	//converter functions
	static glm::mat4 AiToGlmMat4(aiMatrix4x4 in_mat) {
		glm::mat4 newMatrix = glm::mat4(1.0);
		newMatrix[0][0] = in_mat.a1;
		newMatrix[1][0] = in_mat.b1;
		newMatrix[2][0] = in_mat.c1;
		newMatrix[3][0] = in_mat.d1;

		newMatrix[0][1] = in_mat.a2;
		newMatrix[1][1] = in_mat.b2;
		newMatrix[2][1] = in_mat.c2;
		newMatrix[3][1] = in_mat.d2;

		newMatrix[0][2] = in_mat.a3;
		newMatrix[1][2] = in_mat.b3;
		newMatrix[2][2] = in_mat.c3;
		newMatrix[3][2] = in_mat.d3;

		newMatrix[0][3] = in_mat.a4;
		newMatrix[1][3] = in_mat.b4;
		newMatrix[2][3] = in_mat.c4;
		newMatrix[3][3] = in_mat.d4;
		return newMatrix;
	};
};
