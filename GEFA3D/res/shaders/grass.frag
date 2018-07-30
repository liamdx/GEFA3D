#version 440

in vec2 vTexCoords;

uniform sampler2D GrassTexture;

out vec4 FragColor;

void main()
{
	vec4 texColor = texture(GrassTexture, vTexCoords);
	if(texColor.a <0.25)
	{
		discard;
	}
	FragColor = texColor;
}