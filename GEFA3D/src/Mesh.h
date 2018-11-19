#pragma once

#include "Common.h"
#include "Shader.h"
#include "Transform.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 TexCoords;
	glm::vec3 tangent;
};




class Mesh  {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) 
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		//calcMeshBounds();
		setupMesh();
	};

	void Draw(Shader shader);
	void TestDraw(Shader shader);
	void calcMeshBounds();
	//physics position;

	unsigned int vao, vbo, ibo;
	void setupMesh();
	float xBound, yBound, zBound;

};