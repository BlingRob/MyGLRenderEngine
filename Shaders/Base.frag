#version 450 core

const float PI = 3.14159265359;
const float gamma = 2.2;
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
layout (binding = 10) uniform sampler2DShadow shadowMap;


out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 ViewPos;
    vec2 TexCoords;
    vec3 TangentFragPos;
    vec4 ShadowCoord;
    PointLight light;
} fs_in;

void GetLVH(in PointLight,out vec3,out vec3,out vec3);
float GetAttenuation(PointLight);

vec3 PhongLight(PointLight,Material,Texture);
vec3 ImproveLight(PointLight,Material,Texture,float);

float DistributionGGX(vec3, vec3, float);
float GeometrySchlickGGX(float, float);
float GeometrySmith(vec3, vec3, vec3, float);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float ShadowCalculation(vec4);

void main()
{ 
    //FragColor =  vec4(PhongLight(fs_in.light, mat[0], tex[0]),1.0f);// + vec4(texture(tex[0].emissive, fs_in.TexCoords).rgb,0.0f);//texture(tex[0].diffuse,fs_in.TexCoords);//
    //FragColor = vec4(ImproveLight(fs_in.light,mat[0], tex[0]),1.0f) * (1 - ShadowCalculation(fs_in.ShadowCoord));
    //FragColor = vec4(ImproveLight(fs_in.light,mat[0], tex[0]),1.0f) * ShadowCalculation(fs_in.ShadowCoord);
    //FragColor = vec4(ImproveLight(fs_in.light,mat[0], tex[0]),1.0f) * textureProj(shadowMap,fs_in.ShadowCoord);
    //FragColor = vec4(textureProj(shadowMap,fs_in.ShadowCoord/fs_in.ShadowCoord.w));
    //FragColor = vec4(texture(tex[0].normal, fs_in.TexCoords).rgb,0.0f);
    //FragColor = vec4(vec3(textureProj(shadowMap,fs_in.FragPosLightSpace)),0.0f);
    //FragColor = vec4(vec3(ShadowCalculation(fs_in.ShadowCoord)),0.0f);
    FragColor = vec4(ImproveLight(fs_in.light,mat[0], tex[0],ShadowCalculation(fs_in.ShadowCoord)),1.0f);
    //FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}


float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    fragPosLightSpace /= fragPosLightSpace.w;
    //vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    //projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = textureProj(shadowMap,fragPosLightSpace);//texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    //float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    //float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    closestDepth += textureProjOffset(shadowMap,fragPosLightSpace,ivec2(-1,-1));
    closestDepth += textureProjOffset(shadowMap,fragPosLightSpace,ivec2(-1,1));
    closestDepth += textureProjOffset(shadowMap,fragPosLightSpace,ivec2(1,-1));
    closestDepth += textureProjOffset(shadowMap,fragPosLightSpace,ivec2(1,1));


    return closestDepth * 0.2;
}
void GetLVH(in PointLight light,out vec3 L,out vec3 V,out vec3 H)
{
   //fall vector
   L = normalize(light.position - fs_in.TangentFragPos);
   //view vector
   V = normalize(fs_in.ViewPos - fs_in.TangentFragPos);
   //half-way vector
   H = normalize(V + L);
}
float GetAttenuation(PointLight light)
{
    float distance    = length(light.position - fs_in.TangentFragPos);
    return 1.0f / (light.constant + (light.linear + light.quadratic) * distance);
}

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

vec3 ImproveLight(PointLight light,Material mat,Texture tex,float shadow)
{
    vec3 N = normalize((2.0f * texture(tex.normal,fs_in.TexCoords).rgb) - 1.0f);  
    vec3 Lo = vec3(0.0);

    vec3 L,V,H;
    GetLVH(light,L,V,H);

    vec3 albedo = pow(texture(tex.diffuse, fs_in.TexCoords).rgb, vec3(gamma));
    float metallic = texture(tex.metallic_roughness, fs_in.TexCoords).b;
    float roughness = texture(tex.metallic_roughness, fs_in.TexCoords).g;
    float ao        = texture(tex.ao, fs_in.TexCoords).r;
    vec3 F0 = vec3(0.04); 
    F0      = mix(F0, albedo, metallic);
  
    float attenuation = GetAttenuation(light);
    vec3 radiance     = light.ambient * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);       
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0); 

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular     = numerator / denominator; 
    //Fresnel coefficient
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
  
    kD *= 1.0 - metallic;
    float NdotL = max(dot(N, L), 0.0);        
    Lo = (kD * mat.ambient / PI + specular) * radiance * NdotL;

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo * shadow;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/gamma));
     
     return color;
}

vec3 PhongLight(PointLight light,Material mat,Texture tex)
{
    // ���������
    float attenuation = GetAttenuation(light);

    vec4 texColor = texture(tex.diffuse,fs_in.TexCoords);
    vec3 normal = normalize((2.0f * texture(tex.normal,fs_in.TexCoords).rgb) - 1.0f); 

    vec3 L,V,H;
    GetLVH(light,L,V,H);

    // ��������� ���������
    vec3 diffuse = light.diffuse * mat.diffuse * max(dot(normal.xyz, L), 0.0) * texColor.rgb;
    //������� ���������
    vec3 ambient =  light.ambient  * mat.ambient * texColor.rgb;
    //ambAndDiff *= attenuation;
    // ��������� ���������� ������
    vec3 reflectDir = reflect(-L, normal.xyz);
    vec3 specular = light.specular * mat.specular * pow(max(dot(V, H), 0.0), mat.shininess);
    //spec *= attenuation;
    // ����������� ����������
    //vec3 Tex = vec3(texture(texture_diffuse1, fs_in.TexCoords));
    //vec3 ambient  = light.ambient  * texColor.rgb;
    //vec3 diffuse  = light.diffuse  * mat.ambient * pow(texColor.rgb,vec3(2.2));
    //vec3 specular = light.specular * spec * vec3(texture(texture_specular1, fs_in.TexCoords));

    //float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    //return vec4((ambient  + (1.0 - shadow) * (diffuse + specular)) * Tex, 1.0f);
    return vec3(ambient  + diffuse + specular) * attenuation;
}