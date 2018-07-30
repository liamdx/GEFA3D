
#version 440


layout (location=0) in vec3 position;
layout (location=1) in vec3 inNormal;
layout (location=2) in vec2 texCoords;

out vec3 normal;
out vec3 fragPosition;
out vec2 vTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
	fragPosition = vec3(model * vec4(position,1.0));
	normal = mat3(transpose(inverse(model))) * inNormal;
	vTexCoords = texCoords;
	gl_PointSize = gl_Position.z;  
}