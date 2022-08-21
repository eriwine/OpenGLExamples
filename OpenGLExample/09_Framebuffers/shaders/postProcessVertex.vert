#version 450 core

//Input - vertex attributes
layout (location = 0) in vec3 aPos; //Position
layout (location = 1) in vec2 aTexCoord; //Tex coord

//Uniforms

//Passed to fragment shader
out vec2 TexCoord;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = vec4(aPos,1.0);
}


