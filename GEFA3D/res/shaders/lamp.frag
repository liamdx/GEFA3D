#version 440

uniform vec3 color;

out vec4 FragColor;

void main()
{
	FragColor = vec4(color / 10, 1.0);
}