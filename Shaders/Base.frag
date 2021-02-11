#version 450 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    mat3 TBN;
} fs_in;

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

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Texture
{
    sampler2D diffuse;
    sampler2D normal;
    sampler2D specular;
    sampler2D emissive;
    sampler2D height;
};


uniform PointLight light;
uniform Material mat[1];
uniform Texture tex[1];

uniform vec3 viewPos;

//layout (binding = 10) uniform sampler2D shadowMap;

void PhongLight(PointLight,Material,Texture,out vec3,out vec3);
//float ShadowCalculation(vec4);

void main()
{ 
    vec3 ambAndDiff,spec;
    vec4 texColor = texture(tex[0].diffuse,fs_in.TexCoords);
    PhongLight(light,mat[0],tex[0],ambAndDiff,spec);
    FragColor = vec4(ambAndDiff,1.0f) * texColor + vec4(spec,1.0f) + texture(tex[0].emissive, fs_in.TexCoords);
}

/*float ShadowCalculation(vec4 fragPosLightSpace)
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
}*/


void PhongLight(PointLight light,Material mat,Texture tex,out vec3 ambAndDiff,out vec3 spec)
{
    //vec3 viewDir = fs_in.TBN * normalize(viewPos - fs_in.FragPos);
    //vec3 lightDir = normalize(fs_in.TBN * (light.position - fs_in.FragPos));
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);

    vec3 normal = normalize(fs_in.TBN * normalize(2 * texture(tex.normal,fs_in.TexCoords).rgb - 1.0f));
    
    vec3 halfwayDir = normalize(lightDir + viewDir);
    // диффузное освещение
    ambAndDiff += light.diffuse * mat.diffuse * max(dot(normal.xyz, lightDir), 0.0) + light.ambient  * mat.ambient;

    // освещение зеркальных бликов
    vec3 reflectDir = -reflect(-lightDir, normal.xyz);
    spec = mat.specular * pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);

    // затухание
    //float distance    = length(light.position - fs_in.FragPos);
    //float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance); 
    //float attenuation = 1.0f;

    // комбинируем результаты
    //vec3 Tex = vec3(texture(texture_diffuse1, fs_in.TexCoords));
    //vec3 ambient  = light.ambient  * Tex;
    //vec3 diffuse  = light.diffuse  * diff * pow(Tex,vec3(2.2));
    //vec3 specular = light.specular * spec * vec3(texture(texture_specular1, fs_in.TexCoords));
    //ambient  *= attenuation;
    //diffuse  *= attenuation;
    //specular *= attenuation;


    //float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    //return vec4((ambient  + (1.0 - shadow) * (diffuse + specular)) * Tex, 1.0f);
    //return vec4(ambient  + diffuse + specular, 1.0f);
}