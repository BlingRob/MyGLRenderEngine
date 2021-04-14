#version 450 core


const float PI = 3.14159265359;
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

    sampler2D metallic_roughness;
    //sampler2D roughness;
    sampler2D ao;
};

uniform Material mat[1];
uniform Texture tex[1];

out vec4 FragColor;

in VS_OUT {
    vec3 ViewDir;
    vec3 WorldPos;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    PointLight light;
} fs_in;

//layout (binding = 10) uniform sampler2D shadowMap;

vec3 PhongLight(PointLight,Material,Texture);

float DistributionGGX(vec3, vec3, float);
float GeometrySchlickGGX(float, float);
float GeometrySmith(vec3, vec3, vec3, float);
vec3 ImproveLight(PointLight,Material,Texture);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
//float ShadowCalculation(vec4);

void main()
{ 
    //FragColor =  vec4(PhongLight(fs_in.light, mat[0], tex[0]),1.0f);// + vec4(texture(tex[0].emissive, fs_in.TexCoords).rgb,0.0f);//texture(tex[0].diffuse,fs_in.TexCoords);//
    FragColor = vec4(ImproveLight(fs_in.light,mat[0], tex[0]),1.0f);
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

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a     = pow(roughness,4);
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a;
    float denom  = (NdotH2 * (a - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
  
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 ImproveLight(PointLight light,Material mat,Texture tex)
{
    vec3 N = normalize((2.0f * texture(tex.normal,fs_in.TexCoords).rgb) - 1.0f);  
    vec3 Lo = vec3(0.0);

    vec3 L = normalize(light.position - fs_in.WorldPos);
    vec3 H = normalize(fs_in.ViewDir + L);

    vec3 albedo = pow(texture(tex.diffuse, fs_in.TexCoords).rgb, vec3(2.2));
    float metallic = texture(tex.metallic_roughness, fs_in.TexCoords).b;
    float roughness = texture(tex.metallic_roughness, fs_in.TexCoords).g;
    float ao        = texture(tex.ao, fs_in.TexCoords).r;
  
    float distance    = length(light.position - fs_in.WorldPos);
    //float attenuation = 1.0 / (distance * distance);
    //float distance    = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance);
    vec3 radiance     = light.ambient * attenuation;

    vec3 F0 = vec3(0.04); 
    F0      = mix(F0, albedo, metallic);
    vec3 F  = fresnelSchlick(max(dot(H, fs_in.ViewDir), 0.0), F0);

    float NDF = DistributionGGX(N, H, roughness);       
    float G   = GeometrySmith(N, fs_in.ViewDir, L, roughness); 

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, fs_in.ViewDir), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular     = numerator / denominator; 

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
  
    kD *= 1.0 - metallic;
    float NdotL = max(dot(N, L), 0.0);        
    Lo = (kD * mat.ambient / PI + specular) * radiance * NdotL;

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
     
     return color;
}

vec3 PhongLight(PointLight light,Material mat,Texture tex)
{
    // ���������
    //float distance    = length(light.position - fs_in.FragPos);
    //float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance); 

    //vec3 viewDir = fs_in.TBN * normalize(viewPos - fs_in.FragPos);
    //vec3 lightDir = normalize(fs_in.TBN * (light.position - fs_in.FragPos));
    //vec3 viewDir = normalize(fs_in.viewPos - fs_in.FragPos);
    //vec3 lightDir = normalize(light.position - fs_in.FragPos);

    vec4 texColor = texture(tex.diffuse,fs_in.TexCoords);

    vec3 normal = normalize((2.0f * texture(tex.normal,fs_in.TexCoords).rgb) - 1.0f);  
    vec3 halfwayDir = normalize(light.LightDir + fs_in.ViewDir);

    // ��������� ���������
    vec3 diffuse = light.diffuse * mat.diffuse * max(dot(normal.xyz, light.LightDir), 0.0) * texColor.rgb;
    //������� ���������
    vec3 ambient =  light.ambient  * mat.ambient * texColor.rgb;
    //ambAndDiff *= attenuation;
    // ��������� ���������� ������
    vec3 reflectDir = reflect(-light.LightDir, normal.xyz);
    vec3 specular = light.specular * mat.specular * pow(max(dot(fs_in.ViewDir, reflectDir), 0.0), mat.shininess);
    //spec *= attenuation;
    // ����������� ����������
    //vec3 Tex = vec3(texture(texture_diffuse1, fs_in.TexCoords));
    //vec3 ambient  = light.ambient  * texColor.rgb;
    //vec3 diffuse  = light.diffuse  * mat.ambient * pow(texColor.rgb,vec3(2.2));
    //vec3 specular = light.specular * spec * vec3(texture(texture_specular1, fs_in.TexCoords));

    //float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    //return vec4((ambient  + (1.0 - shadow) * (diffuse + specular)) * Tex, 1.0f);
    return vec3(ambient  + diffuse + specular);
}