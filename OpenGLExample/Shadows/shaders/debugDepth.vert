#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec2 aTexCoords;

out vec2 TexCoords;
uniform vec3 scale;
uniform vec3 offset;
void main()
{
    TexCoords = aTexCoords;
    vec3 pos = aPos * scale + offset;
    gl_Position = vec4(pos, 1.0);
}