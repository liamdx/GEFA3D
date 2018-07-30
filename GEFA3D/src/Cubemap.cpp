#include "Cubemap.h"

Cubemap::Cubemap(std::vector<std::string> textures_faces)
{
	//Cube 
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);

	//Cubemap texture
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	int width, height, nrChannels;
	unsigned char* data;
	for (unsigned int i = 0; i < textures_faces.size(); i++)
	{
		data = stbi_load(textures_faces[i].c_str(), &width, &height, &nrChannels, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "failed to load cubemap texture" << std::endl;
			stbi_image_free(data);
		}
		
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

}

void Cubemap::Draw(Shader shader,glm::mat4 cameraViewMatrix, glm::mat4 projectionMatrix)
{
	glm::mat4 skyboxview = glm::mat4(glm::mat3(cameraViewMatrix));
	glDepthFunc(GL_LEQUAL);
	shader.use();
	shader.setMat4("projection", projectionMatrix);
	shader.setMat4("view", skyboxview);

	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	
	glDepthFunc(GL_LESS);
	//glDepthMask(GL_TRUE);
}

void Cubemap::Bind(Shader shader)
{
	//this will DEFINITELY need reworking, especially when PBR comes in to play. currently GL3 e.g. Diffuse = GL0, Specular = GL1, Reflection map = GL2, Skybox = GL3
	shader.use();
	glActiveTexture(GL_TEXTURE3);
	shader.setInt("skybox", 3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	glActiveTexture(GL_TEXTURE0);
}