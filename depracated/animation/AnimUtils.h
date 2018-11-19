#pragma once

#include "Common.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"



//converter functions
static glm::mat4 AiToGlmMat4(aiMatrix4x4 in_mat)
{
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
}

static aiMatrix4x4 GLMMat4ToAi(glm::mat4 mat)
{
	return aiMatrix4x4(mat[0][0], mat[0][1], mat[0][2], mat[0][3],
		mat[1][0], mat[1][1], mat[1][2], mat[1][3],
		mat[2][0], mat[2][1], mat[2][2], mat[2][3],
		mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
}