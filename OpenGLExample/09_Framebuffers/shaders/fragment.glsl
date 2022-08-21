#version 450 core

//Output is just a color
out vec4 FragColor;

//Input from vertex shader
in vec3 Color;
in vec3 WorldNormal;
in vec3 WorldPos;

//Uniforms
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 eyePos;

void main()
{
    //Ambient
    float ambientK = 0.1;
    vec3 ambient = lightColor * ambientK;

    vec3 lightDir = normalize(lightPos - WorldPos);

    //Diffuse
    vec3 worldNorm = normalize(WorldNormal);
    float diffuseFactor = max(dot(worldNorm,lightDir),0);
    vec3 diffuse = lightColor * diffuseFactor;

    //Specular
    float specularK = 0.5;
    vec3 viewDir = normalize(eyePos - WorldPos);
    vec3 reflectDir = reflect(-lightDir,worldNorm);
    float specFactor = pow(max(dot(viewDir,reflectDir),0.0),32);
    vec3 specular = lightColor * specFactor * specularK;

    vec3 col = Color * (ambient + diffuse + specular);
    FragColor = vec4(col,1.0f);
}

