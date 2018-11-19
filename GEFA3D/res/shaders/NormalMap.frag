#version 440

struct Material 
{
	sampler2D m_Diffuse;
	sampler2D m_Specular;
	sampler2D m_Reflection;
    sampler2D m_Normal;
	float m_Shininess;
};

in VS_OUT {
	vec3 fragPosition;
	vec2 vTexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

out vec4 FragColor;

uniform Material mat;


void main()
{
    
    //old normal method
    //vec3 norm = normalize(fs_in.normal);

    // obtain normal from normal map in range [0,1]
    vec3 norm = texture(mat.m_Normal, fs_in.vTexCoords).rgb;
    // transform normal vector to range [-1,1]
    norm = normalize(norm * 2.0 - 1.0);  
    
    //Diffuse texture
    vec3 color = texture(mat.m_Diffuse, fs_in.vTexCoords).rgb;

    // ambient
    vec3 ambient = 0.1 * color;

    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diff * color;

    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
   // vec3 specular = spec * vec3(0.2);
    vec3 specular = spec * vec3(texture(mat.m_Specular, fs_in.vTexCoords));
    specular *= vec3(0.2);

    //attenuation
    float distance = length(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float attenuation = 1.0 / (1.0 + 0.35 * distance + 0.44 * (distance * distance)); // pointlight with attenuation of 13 as on opengl tutorials

    vec3 lighting = ambient + diffuse + specular;
    lighting *= attenuation;
    lighting *= 10;
    
    FragColor = vec4(lighting, 1.0);
}

 