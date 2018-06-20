#pragma once

#include "Common.h"
#include "Shader.h"

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


class Mesh {

	

public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(Shader shader);

private:
	unsigned int vao, vbo, ibo;
	void setupMesh();
};