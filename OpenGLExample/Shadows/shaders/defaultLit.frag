#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 WorldPosition;
in vec4 LightSpacePosition;

uniform sampler2D u_texture;
uniform samplerCube u_skyboxTexture;
uniform sampler2D u_shadowMap;

uniform vec3 u_cameraPos;
uniform vec3 u_lightColor;
uniform vec3 u_lightPos;
uniform vec2 u_tile;

//Returns 1 if out of shadow, 0 if in shadow
float calculateShadow(vec4 lightSpacePos){
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;

    //Closest depth value from light's perspective (0-1)
    float closestDepth = texture(u_shadowMap,projCoords.xy).r;
    //Depth of current fragment from light's perspective (0-1)
    float currentDepth = projCoords.z;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(u_lightPos - WorldPosition);

    float bias = max(0.02 * (1.0 - dot(normal,lightDir)),0.002);

    //PCF filtering
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

void main()
{
    vec3 shadowColor = vec3(0.5,0.6,1.0);
    vec3 textureColor = texture(u_texture,TexCoords * u_tile).rgb;
    float ambientK = 0.25;
    vec3 ambient = textureColor * shadowColor * ambientK;

    vec3 lightDir = normalize(u_lightPos - WorldPosition);

    //Diffuse
    vec3 worldNorm = normalize(Normal);
    float diffuseFactor = max(dot(worldNorm,lightDir),0);
    vec3 diffuse = textureColor * u_lightColor * diffuseFactor;

    //Specular
    float specularK = 0.2;
    vec3 viewDir = normalize(u_cameraPos - WorldPosition);
    vec3 reflectDir = reflect(-lightDir,worldNorm);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specFactor = pow(max(dot(worldNorm,halfwayDir),0.0),16);
    vec3 specular = u_lightColor * specFactor * specularK;
     
    vec3 toCamera = normalize(WorldPosition - u_cameraPos);
    vec3 reflectionDir = reflect(toCamera,normalize(Normal));
    vec3 skyboxColor = texture(u_skyboxTexture,reflectionDir).rgb;

    float shadow = calculateShadow(LightSpacePosition);
    
    vec3 col = (ambient + ((1.0-shadow) * (diffuse + specular)));
    FragColor = vec4(col,1.0f);
}