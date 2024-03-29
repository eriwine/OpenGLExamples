#version 330 core
//The layout here must match the vertex attributes defined in main.cpp
layout (location = 0) in vec3 aPos; //Position
layout (location = 1) in vec3 aNormal; //Normal
layout (location = 2) in vec2 aTexCoord; //Tex coords

out vec3 Normal;
out vec3 WorldPosition;
out vec2 TexCoords;

uniform mat4 u_model;
uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
    TexCoords = aTexCoord;
    Normal = mat3(transpose(inverse(u_model)))*aNormal;
    WorldPosition = vec3(u_model * vec4(aPos,1.0));
    gl_Position = u_projection * u_view * vec4(WorldPosition, 1.0);
}