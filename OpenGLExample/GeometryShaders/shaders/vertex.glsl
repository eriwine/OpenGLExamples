#version 450 core
//The layout here must match the vertex attributes defined in main.cpp
layout (location = 0) in vec3 aPos; //Position
layout (location = 1) in vec4 aColor; //Color

out vec4 ourColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
}