#version 450 core

//Output is just a color
out vec4 FragColor;

//Input from vertex shader
in vec2 TexCoord;

//Uniforms
uniform sampler2D mainTexture;

const float offset = 1.0 / 200.0;

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset), vec2(0.0, offset), vec2(offset, offset),
        vec2(-offset,0.0),       vec2(0.0,0.0),   vec2(offset,0.0),
        vec2(-offset,-offset),  vec2(0.0,-offset),vec2(offset,-offset)
    );

    float kernel[9] = float[](
        1.0/16,2.0/16,1.0/16,
        2.0/16,4.0/16,2.0/16,
        1.0/16,2.0/16,1.0/16
    );

    vec3 col = vec3(0.0);
    for(int i =0; i < 9; i++){
        col+=vec3(texture(mainTexture,TexCoord.st + offsets[i])) * kernel[i];
    }
    FragColor = vec4(col,1.0);
}

