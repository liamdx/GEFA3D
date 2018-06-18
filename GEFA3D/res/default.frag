#version 440


in vec2 vTexCoords;

out vec4 FragColor;

uniform sampler2D fTexture1;
uniform sampler2D fTexture2;
uniform float offset;

void main()
{
	FragColor = mix(texture(fTexture1, vTexCoords), texture(fTexture2, vTexCoords), offset);
}