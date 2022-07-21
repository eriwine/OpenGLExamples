#version 330 core
//The layout here must match the vertex attributes defined in main.cpp
layout (location = 0) in vec3 aPos; //Position
layout (location = 1) in vec3 aColor; //Color
layout (location = 2) in vec2 aTexCoord; //Tex coords

out vec3 Color;
out vec2 TexCoords;

void main()
{
    Color = aColor;
    TexCoords = aTexCoord;
    gl_Position = vec4(aPos, 1.0);
}