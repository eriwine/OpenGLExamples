#version 330 core
//The layout here must match the vertex attributes defined in main.cpp
layout (location = 0) in vec3 aPos; //Position
layout (location = 1) in vec3 aColor; //Color

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 color;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    color = aColor;
}