#version 440
out vec4 FragColor;
  
in vec2 vTexCoords;

uniform samplerCube depthMap;

void main()
{             
    float depthValue = texture(depthMap, vec3(vTexCoords,1.0)).r;
    FragColor = vec4(vec3(depthValue), 1.0);
}  