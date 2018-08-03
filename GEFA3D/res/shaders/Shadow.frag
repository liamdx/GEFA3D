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
	vec4 fragPosLightSpace;
} fs_in;

out vec4 FragColor;

uniform sampler2D shadowMap;
uniform samplerCube pointShadowMap;

uniform Material mat;

uniform vec3 viewPosition;
uniform vec3 lightPosition;
uniform vec3 pointLightPosition;

uniform float near_plane;
uniform float far_plane;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);  

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

float calcPointShadow(vec3 FragPosition)
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

//float LinearizeDepth(float depth)
//{
//    float z = depth * 2.0 - 1.0; // Back to NDC 
//    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
//}

void main()
{
    vec3 color = texture(mat.m_Diffuse, fs_in.vTexCoords).rgb;

    // obtain normal from normal map in range [0,1]
    //vec3 norm = texture(mat.m_Normal, fs_in.vTexCoords).rgb;
    vec3 norm = normalize(fs_in.normal);
    // transform normal vector to range [-1,1]
    //norm = normalize(norm * 2.0 - 1.0);   

    vec3 lightColor = vec3(1.0);
    vec3 lightPos = lightPosition;
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(pointLightPosition - fs_in.fragPosition);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPosition - fs_in.fragPosition);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor * vec3(texture(mat.m_Specular, fs_in.vTexCoords));
    
    // calculate shadow
    float shadow = calcShadow(fs_in.fragPosLightSpace, lightDir, norm);
    float pointShadow = calcPointShadow(fs_in.fragPosition);
    //float shadow = calcPointShadow(fs_in.fragPosition);       
    vec3 lighting1 = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;  
    vec3 lighting2 = (ambient + (1.0 - pointShadow) * (diffuse + specular)) * color;      
    vec3 lighting = (lighting1 + lighting2) / 2;

    FragColor = vec4(lighting, 1.0);
}