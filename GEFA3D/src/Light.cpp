#include "Light.h"

Light::Light() 
{
	//light vao
	unsigned int lightVao = 0;
	unsigned int lightVbo = 0;
	glGenVertexArrays(1, &lightVao);
	glBindVertexArray(lightVao);
	glBindBuffer(GL_ARRAY_BUFFER, lightVbo);
	//Create a vertex attribute (layout = 0 in shader)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

Light::~Light()
{

}