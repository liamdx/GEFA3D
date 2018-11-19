#version 440


layout (location=0) in vec3 position;
layout (location=1) in vec3 inNormal;
layout (location=2) in vec2 texCoords;
layout (location=3) in vec2 tangents;
layout (location=4) in vec2 bitangents;

out VS_OUT {
	vec3 normal;
	vec3 fragPosition;
	vec2 vTexCoords;
	vec4 fragPosLightSpace;
} fs_in;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
	fs_in.fragPosition = vec3(model * vec4(position,1.0));
	fs_in.normal = mat3(transpose(inverse(model))) * inNormal;
	fs_in.vTexCoords = texCoords;
	fs_in.fragPosLightSpace = lightSpaceMatrix * vec4(fs_in.fragPosition,1.0);
	gl_Position = projection * view * model * vec4(position, 1.0);
	
}