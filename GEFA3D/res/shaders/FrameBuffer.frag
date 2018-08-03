#version 440

out vec4 FragColor;

in vec2 vTexCoords;

uniform sampler2D screenTexture;
uniform float gamma;

const float offset = 1.0 / 300.0; 

vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

vec3 col = vec3(0.0);



void main()
{
	//FragColor = texture(screenTexture, vTexCoords);

	

	float kernel[9] = float[] 
    (0,0,0, 0,1,0, 0,0,0);

    //float kernel[9] = float[](
    //    -1, -1, -1,
    //    -1,  9, -1,
    //    -1, -1, -1
    //);
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, vTexCoords.st + offsets[i]));
    }
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    vec4 fragColor = vec4(col, 1.0);

	FragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));


}