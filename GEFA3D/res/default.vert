#version 440

layout (location=0) in vec3 position;
layout (location=1) in vec2 texCoords;

out vec2 vTexCoords;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(position, 1.0);
	vTexCoords = texCoords;
}