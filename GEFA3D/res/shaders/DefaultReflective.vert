
#version 440


layout (location=0) in vec3 position;
layout (location=1) in vec3 inNormal;
layout (location=2) in vec2 texCoords;

out VS_OUT {
	vec3 normal;
	vec3 fragPosition;
	vec2 vTexCoords;
} vs_out;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vs_out.fragPosition = vec3(model * vec4(position,1.0));
	vs_out.normal = mat3(transpose(inverse(model))) * inNormal;
	vs_out.vTexCoords = texCoords;
	gl_Position = projection * view * model * vec4(position, 1.0);
	
}