#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 WorldPosition;

uniform sampler2D u_texture;
uniform samplerCube u_skyboxTexture;
uniform vec3 u_cameraPos;

void main()
{
    vec3 toCamera = normalize(WorldPosition - u_cameraPos);
    vec3 reflectionDir = reflect(toCamera,normalize(Normal));
    vec3 skyboxColor = texture(u_skyboxTexture,reflectionDir).rgb;
    vec3 diffuse = texture(u_texture,TexCoords).rgb;

    float spec = 1.0- max(dot(-toCamera,Normal),0.0);
    vec3 color = mix(diffuse,skyboxColor,spec);
    FragColor = vec4(color,1.0);
}