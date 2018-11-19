#version 440 

//constants
const int MAX_NO_POINT_LIGHTS = 32;
const int MAX_NO_SPOT_LIGHTS = 32;

//PCF shadows
const vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);  

//Light structs
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

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	sampler2D reflection;
};

in VS_OUT{
	vec3 fragPosition;
	vec2 vTexCoords;
	vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
	vec4 fragPosLightSpace;
} fs_in;

out vec4 FragColor;

uniform DirLight dirLight;
uniform PointLight pointLights[MAX_NO_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_NO_SPOT_LIGHTS];

//Texture uniforms
uniform Material mat;
uniform sampler2D shadowMap;
uniform samplerCube skybox;
//uniform samplerCube pointShadows[8];

//Vector uniforms
uniform vec3 viewPosition;


//float uniforms
uniform float near_plane;
uniform float far_plane;
uniform float ratio = 1.00 / 2.58;
//int uniforms
uniform unsigned int NUMBER_OF_POINT_LIGHTS;
uniform unsigned int NUMBER_OF_SPOT_LIGHTS;


// Shader funcs
float calcShadow(vec4 fragPosLightSpace, vec3 lightDir, vec3 Normal)
{
    //Normal = normalize(Normal);
	// perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005);  
    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0; 

	return shadow;
}

float calcPointShadow(vec3 FragPosition, vec3 pointLightPosition, samplerCube pointShadowMap)
{
    // get vector between fragment position and light position
    vec3 fragToLight = FragPosition - pointLightPosition;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // now test for shadows
    float shadow = 0.0;
    float bias   = 0.15;
    int samples  = 20;
    float viewDistance = length(viewPosition - fs_in.fragPosition);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;  
    for(int i = 0; i < samples; ++i)
    {
       float closestDepth = texture(pointShadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
       closestDepth *= far_plane;   // Undo mapping [0;1]
       if(currentDepth - bias > closestDepth)
       {
          shadow += 1.0;
       }
    }

    shadow /= samples;

    return shadow;
}

vec3 calcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-dirLight.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir,normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

	vec3 color;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	color = vec3(texture(mat.diffuse,fs_in.vTexCoords));
	ambient = dirLight.ambient * color;
	diffuse = dirLight.diffuse * diff * color;
	specular = dirLight.specular * spec * vec3(texture(mat.specular, fs_in.vTexCoords));
	
	// calculate shadow
    float shadow = calcShadow(fs_in.fragPosLightSpace, lightDir, normal);
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

	return (ambient + diffuse + specular);

}

vec3 calcPointLight(PointLight pointLight, vec3 normal, vec3 fragPosition, vec3 viewDir)
{
	vec3 lightDir = normalize(pointLight.position - fragPosition);
	vec3 reflectDir = reflect(-lightDir,normal);

	float diff = max(dot(normal, lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

	float distance = length(pointLight.position - fragPosition);
	float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	


	ambient = pointLight.ambient * vec3(texture(mat.diffuse, fs_in.vTexCoords));
	diffuse = pointLight.diffuse * diff * vec3(texture(mat.diffuse, fs_in.vTexCoords));
	specular = pointLight.specular * spec * vec3(texture(mat.specular, fs_in.vTexCoords));


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
		float spec = pow(max(dot(viewDir, reflectDirection), 0.0), 32.0f);


		vec3 ambient;
		vec3 diffuse;
		vec3 specular;  


	
		ambient = spotLight.ambient * vec3(texture(mat.diffuse, fs_in.vTexCoords));
		diffuse = spotLight.diffuse * diffuseFactor * vec3(texture(mat.diffuse, fs_in.vTexCoords));
		specular = spotLight.specular * spec * vec3(texture(mat.specular, fs_in.vTexCoords)); 
		
		

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
	
		ambient += spotLight.ambient * vec3(texture(mat.diffuse, fs_in.vTexCoords));
		
		ambient *= attenuation;
		return ambient;
	}
}

vec3 calcReflection(vec3 Normal, vec3 FragPosition) {

	vec3 I = normalize(FragPosition - viewPosition);
    vec3 R = refract(I, Normal, ratio);

	//vec3 reflection = texture(skybox, R).rgb * vec3(texture(mat.m_Reflection, fs_in.vTexCoords));
	
	vec3 reflection = vec3(texture(skybox, R).rgb);
	vec3 reflectionMap = vec3(texture(mat.reflection, fs_in.vTexCoords).rgb);
	vec3 fReflect =  reflection * reflectionMap;
	return fReflect;
}
	
void main()
{
	 // obtain normal from normal map in range [0,1]
    vec3 norm = texture(mat.normal, fs_in.vTexCoords).rgb;
    // transform normal vector to range [-1,1]
    norm = normalize(norm * 2.0 - 1.0);  
	//get view direction
	vec3 viewDirection = normalize(viewPosition - fs_in.fragPosition);

	vec3 result = calcDirLight(dirLight,norm,viewDirection);

	if(NUMBER_OF_POINT_LIGHTS > 0)
	{
		for(int i =0; i < NUMBER_OF_POINT_LIGHTS; i++)
		{
			result += calcPointLight(pointLights[i], norm, fs_in.fragPosition, viewDirection);
		}
	}
	
	if(NUMBER_OF_SPOT_LIGHTS > 0)
	{
		for(int i =0; i < NUMBER_OF_SPOT_LIGHTS; i++)
		{
			result += calcSpotLight(spotLights[i], norm, fs_in.fragPosition, viewDirection);
		}
	}

	result += calcReflection(norm, fs_in.fragPosition);

	FragColor = vec4(result, 1.0);
}