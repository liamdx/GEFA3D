#version 440

out vec4 FragColor;

in vec2 vTexCoords;
in vec4 GLPosition;

vec4 outlineColor = (0.81, 0.22,0.45,1.0);

void main()
{
	FragColor = outlineColor;	
}