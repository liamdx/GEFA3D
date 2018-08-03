#version 440


layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoords;

out vec2 vTexCoords;
uniform float scaler;
uniform float offsetX;
uniform	float offsetY;

void main()
{
	gl_Position = vec4(position.x * scaler + offsetX, position.y * scaler + offsetY, 0.0, 1.0);
	vTexCoords = texCoords;
}