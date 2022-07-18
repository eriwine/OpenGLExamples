#version 330 core
//The layout here must match the vertex attributes defined in main.cpp
layout (location = 0) in vec3 aPos; //Position
layout (location = 1) in vec3 aColor; //Color
layout (location = 2) in vec3 aNormal; //Normal

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 WorldPos;
out vec3 WorldNormal;
out vec3 Color;

void main()
{
    WorldPos = vec3(model * vec4(aPos,1.0));
    WorldNormal = mat3(transpose(inverse(model)))*aNormal;
    Color = aColor;
    gl_Position = projection * view * vec4(WorldPos,1.0);
}