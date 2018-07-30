#version 440

struct Material 
{
	sampler2D m_Diffuse;
	sampler2D m_Specular;
	float m_Shininess;
};

struct SpotLight 
{
    vec3 position;
	vec3 direction;
	float innerCutoff;
	float outerCutoff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float intensity;

	float constant;
	float linear;
	float quadratic;
};

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;
	float intensity;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

in vec3 normal;
in vec3 fragPosition;
in vec2 vTexCoords;

out vec4 FragColor;

uniform Material mat;

uniform int NOSL;
uniform int NOPL;
uniform int NUMBER_OF_TEXTURES;

uniform PointLight pointLights[32];
uniform DirLight dirLight;
uniform SpotLight spotLights[32];
uniform vec3 viewPosition;

vec3 calcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-dirLight.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir,normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.m_Shininess);

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;


	ambient = dirLight.ambient * vec3(texture(mat.m_Diffuse, vTexCoords));
	diffuse = dirLight.diffuse * diff * vec3(texture(mat.m_Diffuse, vTexCoords));
	specular = dirLight.specular * spec * vec3(texture(mat.m_Specular, vTexCoords));
	

	return (ambient + diffuse + specular);

}

vec3 calcPointLight(PointLight pointLight, vec3 normal, vec3 fragPosition, vec3 viewDir)
{
	vec3 lightDir = normalize(pointLight.position - fragPosition);
	vec3 reflectDir = reflect(-lightDir,normal);

	float diff = max(dot(normal, lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.m_Shininess);

	float distance = length(pointLight.position - fragPosition);
	float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	


	ambient = pointLight.ambient * vec3(texture(mat.m_Diffuse, vTexCoords));
	diffuse = pointLight.diffuse * diff * vec3(texture(mat.m_Diffuse, vTexCoords));
	specular = 3*(pointLight.specular * spec * vec3(texture(mat.m_Specular, vTexCoords)));


	ambient *= pointLight.intensity * attenuation;
	diffuse *= pointLight.intensity * attenuation;
	specular *= pointLight.intensity * attenuation;



	return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight spotLight, vec3 normal, vec3 fragPosition, vec3 viewDir)
{
	
	vec3 lightDir = normalize(spotLight.position - fragPosition);
	float diffuseFactor = max(dot(normal,lightDir),0.0);

	float distance = length(spotLight.position - fragPosition);
	float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));

	float theta = dot(lightDir, normalize(-spotLight.direction));
	float epsilon = spotLight.innerCutoff - spotLight.outerCutoff;
	float intensity = clamp((theta - spotLight.outerCutoff) / epsilon, 0.0,1.0);
	
	if(theta > spotLight.outerCutoff)
	{
		vec3 viewDirection = normalize(viewPosition - fragPosition);
		vec3 reflectDirection = reflect(-lightDir, normal);	
		float spec = pow(max(dot(viewDir, reflectDirection), 0.0), mat.m_Shininess * 128.0f);


		vec3 ambient;
		vec3 diffuse;
		vec3 specular;  


	
		ambient = spotLight.ambient * vec3(texture(mat.m_Diffuse, vTexCoords));
		diffuse = spotLight.diffuse * diffuseFactor * vec3(texture(mat.m_Diffuse, vTexCoords));
		specular = spotLight.specular * spec * vec3(texture(mat.m_Specular, vTexCoords)); 
		
		

		ambient *= attenuation;
		diffuse *= spotLight.intensity;
		diffuse *= attenuation;
		specular *= spotLight.intensity;
		specular *= attenuation;

		return (ambient + diffuse + specular);
	}
	else
	{
	vec3 ambient;
		for(int i = 0; i < NUMBER_OF_TEXTURES; i++)
		{
			ambient += spotLight.ambient * vec3(texture(mat.m_Diffuse, vTexCoords));
		}
		
		ambient *= attenuation;
		return ambient;
	}
	
}

void main()
{
	
	vec3 norm = normalize(normal);
	vec3 viewDirection = normalize(viewPosition - fragPosition);
	vec3 result = calcDirLight(dirLight, norm, viewDirection);

	//result += calcPointLight(debugLight, norm, fragPosition, viewDirection);
	if(NOPL > 0)
	{
	for(int i = 0; i < NOPL; i++)
	{
		result += calcPointLight(pointLights[i], norm, fragPosition, viewDirection);
	}
	}
	


	if(NOSL > 0)
	{
	for(int i = 0; i < NOSL; i++)
	{
		result += calcSpotLight(spotLights[i], norm, fragPosition, viewDirection);
	}
	}
	


	FragColor = vec4(result, 1.0);

	
}

