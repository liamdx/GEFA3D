#include "Texture.h"

Texture::Texture(const char* filepath, unsigned int slot)
	:t_Slot(GL_TEXTURE0 + slot), slot(slot)
{

	glGenTextures(1, &t_Id);
	glActiveTexture(t_Slot);
	glBindTexture(GL_TEXTURE_2D, t_Id);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char *data = stbi_load(filepath, &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
		{
			format = GL_RED;
		}
		else if (nrChannels == 3)
		{
			format = GL_RGB;
		}
		else if(nrChannels == 4)
		{
			format = GL_RGBA;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		stbi_image_free(data);
	}




	
    unsigned int textureID;
    glGenTextures(1, &textureID);


	
}

Texture::~Texture() {
	glDeleteTextures(1, &t_Slot);
	glDeleteBuffers(1, &t_Id);
}

void Texture::Use() {
	glActiveTexture(t_Slot);
	glBindTexture(GL_TEXTURE_2D, t_Id);
}