#version 450 core

//Output is just a color
out vec4 FragColor;

//Input from vertex shader
in vec2 TexCoord;

//Uniforms
uniform sampler2D mainTexture;

void main()
{
    FragColor = texture(mainTexture,TexCoord);
}

