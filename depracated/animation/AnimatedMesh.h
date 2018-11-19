#pragma once

#include "Shader.h"
#include "animation\Bone.h"

struct AnimatedVertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 TexCoords;
	glm::vec3 tangent;
	glm::ivec4 boneIds;
	glm::vec4 boneWeights;
};


class AnimatedMesh {

public:


	unsigned int numBonesPerVertex;

	std::vector<AnimatedVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Skeleton* modelSkeleton;

	//Animator animator

	AnimatedMesh(std::vector<AnimatedVertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		//calcAnimatedMeshBounds();
		setupAnimatedMesh();
	}

	void Draw(Shader shader);
	void TestDraw(Shader shader);
	void calcAnimatedMeshBounds();
	//physics position;

	unsigned int vao, vbo, ibo;
	void setupAnimatedMesh();
	float xBound, yBound, zBound;

};