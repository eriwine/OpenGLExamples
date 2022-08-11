#version 450 core
out vec4 FragColor;

in vec4 vertexColor;
in vec2 texCoord;

void main()
{
    vec4 color = vertexColor * vec4(texCoord.x,texCoord.y,0.0,1.0);
    float a = 1.0 - smoothstep(0.45,0.5,length(texCoord-vec2(0.5)));
    FragColor = color;
}