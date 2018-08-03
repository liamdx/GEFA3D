#version 440


struct Material 
{
	sampler2D m_Diffuse;
	sampler2D m_Specular;
	sampler2D m_Reflection;
	sampler2D m_Normal;
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

	float distance;
};


in VS_OUT {
	vec3 normal;
	vec3 fragPosition;
	vec2 vTexCoords;
} fs_in;

out vec4 FragColor;

uniform Material mat;
uniform samplerCube skybox;
uniform sampler2D shadowMap;

uniform int NOSL;
uniform int NOPL;

uniform PointLight pointLights[32];
uniform DirLight dirLight;
uniform SpotLight spotLights[32];
uniform vec3 viewPosition;

//Reflections
float ratio = 1.00 / 2.58;
uniform float reflectionIntensity;

vec3 calcReflection(vec3 Normal, vec3 FragPosition) {
	vec3 I = normalize(FragPosition - viewPosition);
    vec3 R = refract(I, Normal, ratio);

	//vec3 reflection = texture(skybox, R).rgb * vec3(texture(mat.m_Reflection, fs_in.vTexCoords));
	
	vec3 reflection = vec3(texture(skybox, R).rgb);
	vec3 reflectionMap = vec3(texture(mat.m_Reflection, fs_in.vTexCoords).rgb);
	vec3 fReflect =  reflection * reflectionMap;
	return fReflect;
}
//back to usual

vec3 calcDirLight(DirLight dirLight, vec3 normal, vec3 FragPosition, vec3 viewDir)
{
	//vec3 lightDir = normalize(-dirLight.direction);
	vec3 lightDir = normalize(dirLight.direction - FragPosition);
	vec3 viewDirection = normalize(viewPosition - FragPosition);
	vec3 halfwayDir = normalize(lightDir + viewDirection);

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir,normal);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), mat.m_Shininess);

	
	vec3 ambient = dirLight.ambient * vec3(texture(mat.m_Diffuse, fs_in.vTexCoords));
	vec3 diffuse = dirLight.diffuse * diff * vec3(texture(mat.m_Diffuse, fs_in.vTexCoords));
	vec3 specular = dirLight.specular * spec * vec3(texture(mat.m_Specular, fs_in.vTexCoords));


	return (ambient + diffuse  + specular);

}

vec3 calcPointLight(PointLight pointLight, vec3 normal, vec3 FragPosition, vec3 viewDir)
{
	//vec3 lightDir = normalize(pointLight.position - fragPosition);
	vec3 lightDir = normalize(dirLight.direction - FragPosition);
	vec3 reflectDir = reflect(-lightDir,normal);	
	vec3 viewDirection = normalize(viewPosition - FragPosition);
	vec3 halfwayDir = normalize(lightDir + viewDirection);

	float diff = max(dot(normal, lightDir), 0.0);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), mat.m_Shininess);

	float attenuation = 1.0 / pointLight.distance;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;


	ambient = pointLight.ambient * vec3(texture(mat.m_Diffuse, fs_in.vTexCoords));
	diffuse = pointLight.diffuse * diff * vec3(texture(mat.m_Diffuse, fs_in.vTexCoords));
	specular = pointLight.specular * spec * vec3(texture(mat.m_Specular, fs_in.vTexCoords));


	ambient *= pointLight.intensity * attenuation;
	diffuse *= pointLight.intensity * attenuation;
	specular *= pointLight.intensity * attenuation;


	return (ambient + diffuse  + specular);
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


		
		ambient = spotLight.ambient * vec3(texture(mat.m_Diffuse, fs_in.vTexCoords));
		diffuse = spotLight.diffuse * diffuseFactor * vec3(texture(mat.m_Diffuse, fs_in.vTexCoords));
		specular = spotLight.specular * spec * vec3(texture(mat.m_Specular, fs_in.vTexCoords)); 
				

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

		ambient += spotLight.ambient * vec3(texture(mat.m_Diffuse, fs_in.vTexCoords));
		
		ambient *= attenuation;
		return ambient;
	}
	
}

void main()
{
	vec3 norm = normalize(fs_in.normal);
	vec3 viewDirection = normalize(viewPosition - fs_in.fragPosition);

	vec3 result = calcDirLight(dirLight, norm, fs_in.fragPosition,  viewDirection);

	if(NOPL > 0)
	{
		for(int i = 0; i < NOPL; i++)
		{
			result += calcPointLight(pointLights[i], norm, fs_in.fragPosition, viewDirection);
		}
	}
	
	if(NOSL > 0)
	{
		for(int i = 0; i < NOSL; i++)
		{
			result += calcSpotLight(spotLights[i], norm, fs_in.fragPosition, viewDirection);
		}
	}

	result += calcReflection(norm, viewPosition) * reflectionIntensity;
	FragColor = vec4(result, 1.0);
}

