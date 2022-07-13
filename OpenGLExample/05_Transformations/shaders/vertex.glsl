#version 330 core
//The layout here must match the vertex attributes defined in main.cpp
layout (location = 0) in vec3 aPos; //Position

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}