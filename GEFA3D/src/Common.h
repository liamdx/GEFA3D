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



//External
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

#include <fstream>

#include <sstream>

#include <vector>

#include <iostream> 

#include <memory>

#include <map>

//Image loader (STLI should work too)
#include "ext\stb_image\stb_image.h"

//Physics integration
#include "bounce\bounce.h"

//sound lib
#include "yse.hpp"

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }


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

	//static aiMatrix4x4 GLMMat4ToAi(glm::mat4 mat)
	//{
	//	return aiMatrix4x4(mat[0][0], mat[0][1], mat[0][2], mat[0][3],
	//		mat[1][0], mat[1][1], mat[1][2], mat[1][3],
	//		mat[2][0], mat[2][1], mat[2][2], mat[2][3],
	//		mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
	//}
};
