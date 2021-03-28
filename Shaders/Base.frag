#version 450 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
struct PointLight
 {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //Calc in shader
    vec3 LightDir;
}; 

struct Texture
{
    sampler2D diffuse;
    sampler2D normal;
    sampler2D specular;
    sampler2D emissive;
    sampler2D height;
};

uniform Material mat[1];
uniform Texture tex[1];

out vec4 FragColor;

in VS_OUT {
    vec3 ViewDir;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    PointLight light;
} fs_in;

//layout (binding = 10) uniform sampler2D shadowMap;

vec3 PhongLight(PointLight,Material,Texture);
//float ShadowCalculation(vec4);

void main()
{ 
    FragColor =  vec4(PhongLight(fs_in.light, mat[0], tex[0]),1.0f);// + vec4(texture(tex[0].emissive, fs_in.TexCoords).rgb,0.0f);//texture(tex[0].diffuse,fs_in.TexCoords);//
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


vec3 PhongLight(PointLight light,Material mat,Texture tex)
{
    // затухание
    //float distance    = length(light.position - fs_in.FragPos);
    //float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance); 

    //vec3 viewDir = fs_in.TBN * normalize(viewPos - fs_in.FragPos);
    //vec3 lightDir = normalize(fs_in.TBN * (light.position - fs_in.FragPos));
    //vec3 viewDir = normalize(fs_in.viewPos - fs_in.FragPos);
    //vec3 lightDir = normalize(light.position - fs_in.FragPos);

    vec4 texColor = texture(tex.diffuse,fs_in.TexCoords);

    vec3 normal = normalize((2.0f * texture(tex.normal,fs_in.TexCoords).rgb) - 1.0f);  
    vec3 halfwayDir = normalize(light.LightDir + fs_in.ViewDir);

    // диффузное освещение
    vec3 diffuse = light.diffuse * mat.diffuse * max(dot(normal.xyz, light.LightDir), 0.0) * texColor.rgb;
    //фоновое освещение
    vec3 ambient =  light.ambient  * mat.ambient * texColor.rgb;
    //ambAndDiff *= attenuation;
    // освещение зеркальных бликов
    vec3 reflectDir = reflect(-light.LightDir, normal.xyz);
    vec3 specular = light.specular * mat.specular * pow(max(dot(fs_in.ViewDir, reflectDir), 0.0), mat.shininess);
    //spec *= attenuation;
    // комбинируем результаты
    //vec3 Tex = vec3(texture(texture_diffuse1, fs_in.TexCoords));
    //vec3 ambient  = light.ambient  * texColor.rgb;
    //vec3 diffuse  = light.diffuse  * mat.ambient * pow(texColor.rgb,vec3(2.2));
    //vec3 specular = light.specular * spec * vec3(texture(texture_specular1, fs_in.TexCoords));

    //float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    //return vec4((ambient  + (1.0 - shadow) * (diffuse + specular)) * Tex, 1.0f);
    return vec3(ambient  + diffuse + specular);
}