#version 330 core
out vec4 FragColor;

in vec3 Color;
in vec2 TexCoords;

uniform sampler2D Texture;
uniform float OffsetX;
uniform float OffsetY;

void main()
{
    FragColor = texture(Texture,TexCoords + vec2(OffsetX,OffsetY)) * vec4(Color,1.0);
}