#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}

void Mesh::Draw(Shader shader)
{
	shader.setInt("NUMBER_OF_TEXTURES", textures.size());
	int diffuseCount = 0;
	int specularCount = 0;
	std::stringstream ss;
	std::string s;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		if (textures[i].t_Type == "diffuse")
		{
			ss.clear();
			ss << "mat.m_Diffuse[" << diffuseCount << "]";
			std::cout << ss.str() << std::endl;
			s = ss.str();
			shader.setFloat(s, i);
			diffuseCount += 1;
			glBindTexture(GL_TEXTURE_2D, textures[i].t_Id);
		}
		else if (textures[i].t_Type == "specular")
		{
			ss.clear();
			ss << "mat.m_Specular[" << diffuseCount << "]";
			s = ss.str();
			shader.setFloat(s, i);
			specularCount += 1;
			glBindTexture(GL_TEXTURE_2D, textures[i].t_Id);
		}

		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}
}