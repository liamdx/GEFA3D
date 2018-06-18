#version 440

struct Material 
{
	sampler2D m_Diffuse;
	sampler2D m_Specular;
	float m_Shininess;
};


struct Light {
    vec3 position;
	vec3 direction;
	float innerCutoff;
	float outerCutoff;
  
    vec3 l_Ambient;
    vec3 l_Diffuse;
    vec3 l_Specular;

	float constant;
	float linear;
	float quadratic;
};

in vec3 normal;
in vec3 fragPosition;
in vec2 vTexCoords;

out vec4 FragColor;

uniform Material mat;
uniform Light light;  
uniform vec3 viewPosition;


void main()
{
	
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPosition);
	float diffuseFactor = max(dot(norm,lightDir),0.0);

	float distance = length(light.position - fragPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.innerCutoff - light.outerCutoff;
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0,1.0);
	
	if(theta > light.outerCutoff)
	{
		vec3 viewDirection = normalize(viewPosition - fragPosition);
		vec3 reflectDirection = reflect(-lightDir, norm);	
		float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), mat.m_Shininess * 128.0f);

		vec3 ambient = light.l_Ambient * vec3(texture(mat.m_Diffuse, vTexCoords));
		vec3 diffuse = light.l_Diffuse * diffuseFactor * vec3(texture(mat.m_Diffuse, vTexCoords));
		vec3 specular = 5 * (light.l_Specular * spec * vec3(texture(mat.m_Specular, vTexCoords)));  

		ambient *= attenuation;
		diffuse *= intensity;
		specular *= intensity;

		FragColor = vec4(ambient + diffuse + specular, 1.0);
	}
	else
	{
		vec3 ambient = light.l_Ambient * vec3(texture(mat.m_Diffuse, vTexCoords));
		ambient *= attenuation;
		FragColor = vec4(ambient, 1.0);
	}
	
}