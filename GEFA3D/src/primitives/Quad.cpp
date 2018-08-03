#include "Quad.h"

Quad::Quad(std::string texturePath) {
	tex.t_Path = texturePath;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 *sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),(void*)(3 * sizeof(float)));

	glBindVertexArray(0);
	tex.t_Id = Model::TextureFromFile(tex.t_Path.c_str(), "");
}
Quad::Quad()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);
}
Quad::~Quad()
{

}
void Quad::Draw(Shader shader)
{
	glActiveTexture(GL_TEXTURE0);
	std::string s = "GrassTexture";
	shader.setInt(s, 0);

	glBindTexture(GL_TEXTURE_2D, tex.t_Id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glActiveTexture(GL_TEXTURE0);
}

void Quad::Draw(Shader shader, const char* uniformName, unsigned int textureLocation)
{
	glActiveTexture(GL_TEXTURE0);
	shader.setInt(uniformName, 0);
	glBindTexture(GL_TEXTURE_2D, textureLocation);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glActiveTexture(GL_TEXTURE0);
}

screenQuad::screenQuad()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

}
void screenQuad::Draw(Shader shader, const char* uniformName, unsigned int textureLocation)
{
	glActiveTexture(GL_TEXTURE0);
	shader.setInt(uniformName, 0);
	glBindTexture(GL_TEXTURE_2D, textureLocation);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glActiveTexture(GL_TEXTURE0);
}

screenQuad::~screenQuad()
{

}
//
//std::vector<float> debugQuad::getVertices(quadLocation screenLocation)
//{
//	
//	if (screenLocation == 1)
//	{
//		std::vector<float> vertices = {}
//	}
//	if (screenLocation == 2)
//	{
//		std::vector<float> vertices = {}
//	}
//	if (screenLocation == 3)
//	{
//		std::vector<float> vertices = {}
//	}
//}