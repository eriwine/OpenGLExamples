#version 450 core
out vec4 FragColor;

in vec3 vertexColor;
in vec2 texCoord;

void main()
{
    vec3 color = vertexColor * vec3(texCoord.x,texCoord.y,0.0);
    float a = 1.0 - smoothstep(0.45,0.5,length(texCoord-vec2(0.5)));
    FragColor = vec4(color,a);
}