#version 440


layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 vTexCoords;
uniform float scaler;
uniform float offsetX;
uniform	float offsetY;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = vec4(position, 1.0);
	vTexCoords = texCoords;
}