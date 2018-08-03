#version 440


layout (location=0) in vec3 position;
layout (location=1) in vec3 inNormal;
layout (location=2) in vec2 texCoords;
layout (location=3) in vec3 tangent;
layout (location=4) in vec3 bitangent;

out VS_OUT {
	vec3 normal;
	vec3 fragPosition;
	vec2 vTexCoords;
	vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPosition;
uniform vec3 lightPosition;

void main()
{
	fs_in.fragPosition = vec3(model * vec4(position,1.0));
	fs_in.normal = mat3(transpose(inverse(model))) * inNormal;
	fs_in.vTexCoords = texCoords;

	vec3 T = normalize(vec3(model * vec4(tangent,1.0)));
	vec3 B = normalize(vec3(model * vec4(bitangent,1.0)));
	vec3 N = normalize(vec3(model * vec4(normal,1.0)));
	mat3 TBN = transpose(mat3(T,B,N));

	fs_in.TangentLightPos = TBN * lightPosition;
	fs_in.TangentViewPos = TBN * viewPosition;
	fs_in.TangentFragPos = TBN * vec3(model * vec4(position, 0.0));


	gl_Position = projection * view * model * vec4(position, 1.0);
	
}