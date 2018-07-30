#version 440
out vec4 FragColor;

in vec3 Position;
in vec3 Normal;

uniform vec3 cameraPosition;
uniform samplerCube skybox;

float ratio = 1.00 / 7.33;

void main()
{
    vec3 I = normalize(Position - cameraPosition);
    vec3 R = refract(I, normalize(Normal),ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}