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
	vec3 normal;
	vec3 fragPosition;
	vec2 vTexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

out vec4 FragColor;

uniform Material mat;

uniform vec3 viewPosition;
uniform vec3 lightPosition;
uniform vec3 pointLightPosition;


void main()
{
    vec3 color = texture(mat.m_Diffuse, fs_in.vTexCoords).rgb;
    //old normal method
    //vec3 norm = normalize(fs_in.normal);

    // obtain normal from normal map in range [0,1]
    vec3 norm = texture(mat.m_Normal, fs_in.vTexCoords).rgb;
    // transform normal vector to range [-1,1]
    norm = normalize(norm * 2.0 - 1.0);  
    //norm = normalize(fs_in.TBN * norm);
    
    vec3 lightColor = vec3(1.0);
    vec3 lightPos = lightPosition;
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor * vec3(texture(mat.m_Specular, fs_in.vTexCoords));
    
  
    vec3 lighting = (ambient + diffuse + specular) * color;    
    
    FragColor = vec4(lighting, 1.0);
}