#version 450 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

out vec4 FragColor;

struct PointLight
 {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

uniform PointLight light;
uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_emissive1;

layout (binding = 10) uniform sampler2D shadowMap;

vec4 culcLight();
float ShadowCalculation(vec4);

void main()
{ 
    FragColor = culcLight() + texture(texture_emissive1, fs_in.TexCoords);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}


vec4 culcLight()
{
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    // диффузное освещение
    float diff = max(dot(fs_in.Normal, lightDir), 0.0);

    // освещение зеркальных бликов
    vec3 reflectDir = reflect(-lightDir, fs_in.Normal);
    float spec = pow(max(dot(halfwayDir, reflectDir), 0.0), 100.0f);

    // затухание
    //float distance    = length(light.position - fs_in.FragPos);
    //float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance); 
    float attenuation = 3.0f;

    // комбинируем результаты
    vec3 Tex = vec3(texture(texture_diffuse1, fs_in.TexCoords));
    vec3 ambient  = light.ambient  * Tex;
    vec3 diffuse  = light.diffuse  * diff * pow(Tex,vec3(2.2));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, fs_in.TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;


    //float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    //return vec4((ambient  + (1.0 - shadow) * (diffuse + specular)) * Tex, 1.0f);
    return vec4(ambient  + diffuse + specular, 1.0f);
}