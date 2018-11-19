#version 440


layout (location=0) in vec3 position;
layout (location=1) in vec3 inNormal;
layout (location=2) in vec2 texCoords;
layout (location=3) in vec3 tangent;



out VS_OUT {
	vec3 fragPosition;
	vec2 vTexCoords;
	vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
	vec4 fragPosLightSpace;
} fs_in;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

uniform vec3 viewPosition;
uniform vec3 dirLightPosition;

void main()
{
	fs_in.fragPosition = vec3(model * vec4(position,1.0));
	fs_in.vTexCoords = texCoords;

	mat3 normalMatrix = transpose(inverse(mat3(model)));

	vec3 T = normalize(normalMatrix * tangent);
	vec3 N = normalize(normalMatrix * inNormal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N,T);;
	mat3 TBN = transpose(mat3(T,B,N));

	fs_in.TangentLightPos = TBN * dirLightPosition;
	fs_in.TangentViewPos = TBN * viewPosition;
	fs_in.TangentFragPos = TBN * fs_in.fragPosition;

	fs_in.fragPosLightSpace = lightSpaceMatrix * vec4(fs_in.fragPosition,1.0);


	gl_Position = projection * view * model * vec4(position, 1.0);
}