#pragma once

#include "Common.h"
#include "Shader.h"
#include "Transform.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int t_Id;
	std::string t_Type;
	std::string t_Path;
};


class Mesh : public EngineComponent {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) : EngineComponent("Mesh") 
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->transform = new Transform();

		calcMeshBounds();
		setupMesh();
	};

	void Draw(Shader shader);
	void calcMeshBounds();
	//physics position;
	b3MeshShape collisionShape;
	Transform* transform;
private:
	unsigned int vao, vbo, ibo;
	void setupMesh();
	float xBound, yBound, zBound;

};