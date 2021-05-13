#version 450 core

const float PI = 3.14159265359;
const float gamma = 2.2;
const uint MAX_LIGHTS = 16;
const vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);  


struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;

};

struct Light
 {    
    int index;
    
    vec3 clq;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //mat4 ShadowMatrix;
    //vec4 ShadowCoords;
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
uniform uint NumLights;
uniform Light light[MAX_LIGHTS];
uniform vec4 LightPositions[MAX_LIGHTS];
layout (binding = 10) uniform samplerCubeArray shadowMaps;
//////////////////////////////
layout (binding = 11 ) uniform sampler3D OffsetTex;

uniform float Radius;
uniform vec3 OffsetTexSize; // (width, height, depth)
////////////////////////////////

uniform float far_plane;

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 ViewPos;
    vec2 TexCoords;
    vec3 TangentFragPos;

    vec4 LightPositions[MAX_LIGHTS];
} fs_in;

void GetLVH(in vec4 LightPosition,out vec3 L,out vec3 V,out vec3 H);
float GetAttenuation(in vec4 LightPosition,in Light light);

vec3 PhongLight(Light,Material,Texture);
vec3 ImproveLight(Material,Texture);

float DistributionGGX(vec3, vec3, float);
float GeometrySchlickGGX(float, float);
float GeometrySmith(vec3, vec3, vec3, float);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
float ShadowCalculation(uint);

float VectorToDepth (vec3 Vec)
{
    vec3 AbsVec = abs(Vec);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    // Replace f and n with the far and near plane values you used when
    //   you drew your cube map.
    const float f = far_plane;
    const float n = 0.1;

    float NormZComp = (f+n) / (f-n) - (2*f*n)/(f-n)/LocalZcomp;
    return (NormZComp + 1.0) * 0.5;
}

void main()
{ 
    //FragColor =  vec4(PhongLight(fs_in.light, mat[0], tex[0]),1.0f);// + vec4(texture(tex[0].emissive, fs_in.TexCoords).rgb,0.0f);//texture(tex[0].diffuse,fs_in.TexCoords);//
    //FragColor = vec4(ImproveLight(fs_in.light,mat[0], tex[0]),1.0f) * (1 - ShadowCalculation(fs_in.ShadowCoord));
    //FragColor = vec4(ImproveLight(fs_in.light,mat[0], tex[0]),1.0f) * ShadowCalculation(fs_in.ShadowCoord);
    //FragColor = vec4(ImproveLight(fs_in.light,mat[0], tex[0]),1.0f) * textureProj(shadowMap,fs_in.ShadowCoord);

    //FragColor = vec4(textureProj(shadowMap,fs_in.ShadowCoord/fs_in.ShadowCoord.w));
    //FragColor = vec4(texture(tex[0].normal, fs_in.TexCoords).rgb,0.0f);
    //FragColor = vec4(vec3(textureProj(shadowMap,fs_in.FragPosLightSpace)),0.0f);
    //FragColor = vec4(vec3(ShadowCalculation(0)),1.0f);
    FragColor = vec4(ImproveLight(mat[0], tex[0]),1.0f);
    //FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));

    ///FragColor = vec4(vec3(ShadowCalculation(0)),1.0f);
    ///FragColor += vec4(vec3(ShadowCalculation(1)),1.0f);
    //vec3 fragToLight0 = fs_in.FragPos - vec3(LightPositions[0]); 
    //vec3 fragToLight1 = fs_in.FragPos - vec3(LightPositions[1]); 
    //FragColor = texture(shadowMaps, vec4(fragToLight0, 0.0f));
    //FragColor += texture(shadowMaps, vec4(fragToLight1, 1.0f));
    //FragColor = vec4(1.0f - 1.0/pow(length(fs_in.FragPos - vec3(LightPositions[1])) + 0.0001,0.1f));
    //FragColor += vec4(1.0f - 1.0/(length(fs_in.FragPos - vec3(LightPositions[1]))+ 0.0001));
}


float ShadowCalculation(uint index)
{
    // perform perspective divide
    //fragPosLightSpace /= fragPosLightSpace.w;
    //vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    //projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    //float shadow = textureProj(shadowMap,fragPosLightSpace);//texture(shadowMap, projCoords.xy).r; 

    /*vec3 fragToLight = fs_in.FragPos - vec3(LightPositions[index]); 
    float closestDepth = texture(shadowMaps, vec4(fragToLight, index)).r;
    closestDepth *= far_plane; 
    float currentDepth = length(fragToLight);
    float bias = 0.05; 
    float shadow = currentDepth - bias  > closestDepth ? 0.0 : 1.0;
    return shadow;*/  
    vec3 fragToLight = fs_in.FragPos - vec3(LightPositions[index]); 
    //float closestDepth = texture(shadowMaps, vec4(fragToLight, index)).r;
    //closestDepth *= far_plane; 
    float currentDepth = length(fragToLight);
    //float bias = 0.05; 
    //float shadow = currentDepth - bias  > closestDepth ? 0.0 : 1.0;
    //return shadow;  

    float shadow = 0.0;
    float bias   = 0.15;
    int samples  = 20;
    float viewDistance = length(fs_in.ViewPos - fs_in.TangentFragPos);
    //float diskRadius = 0.05;
    //float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;

    currentDepth /= far_plane;
    //float diskRadius = (1.0 + currentDepth) / 20.0;
    float diskRadius = abs(length(fs_in.FragPos - vec3(LightPositions[index])) / length(far_plane - vec3(LightPositions[index])) );
    for(int i = 0; i < 4; ++i)
    {
        float closestDepth = texture(shadowMaps, vec4(fragToLight + sampleOffsetDirections[i] * diskRadius, index)).r;
        //diskRadius = (1.0 + closestDepth) / 25.0;
       // closestDepth *= far_plane;   // обратное преобразование из диапазона [0;1]
        if(currentDepth > closestDepth)
            shadow += 1.0;
    }
    if(shadow == 4.0f)
        return 1.0f - (shadow / 4.0f);
    for(int i = 4; i < samples; ++i)
    {
        float closestDepth = texture(shadowMaps, vec4(fragToLight + sampleOffsetDirections[i] * diskRadius, index)).r;
        //diskRadius = (1.0 + closestDepth) / 25.0;
       // closestDepth *= far_plane;   // обратное преобразование из диапазона [0;1]
        if(currentDepth > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    return 1 - shadow;
    /*
    for(int i = 0; i <  2.0f * samplesDiv2; ++i)
    {
        offsetCoord.z = i;
        offsets = texelFetch(OffsetTex,offsetCoord,0) * diskRadius;

        closestDepth = texture(shadowMaps, vec4(fragToLight + offsets.xyz, index)).r;
        //closestDepth *= far_plane;   // обратное преобразование из диапазона [0;1]

        if(currentDepth > closestDepth)
            shadow += 1.0;
            //shadow += currentDepth;
    }
    shadow /= float(2.0f * samplesDiv2);
    return 1 - shadow;*/

        /*    ivec3 offsetCoord;
    offsetCoord.xy = ivec2( mod( gl_FragCoord.xy, OffsetTexSize.xy ) );

    float sum = 0.0;
    int samplesDiv2 = int(OffsetTexSize.z);
    vec4 sc = fragPosLightSpace;
    vec2 CurPixel = fragPosLightSpace.xy;

    for( int i = 0 ; i < 4; ++i )
    {
        offsetCoord.z = i;
        vec4 offsets = texelFetch(OffsetTex,offsetCoord,0) * Radius * fragPosLightSpace.w;

        sc.xy = CurPixel + offsets.xy;
        sum += textureProj(shadowMap, sc);
        sc.xy = CurPixel + offsets.zw;
        sum += textureProj(shadowMap, sc);
    }
    float shadow = sum / 8.0;
    
    if( shadow != 1.0 && shadow != 0.0 ) 
    {
        for( int i = 4; i < samplesDiv2; ++i )
         {
            offsetCoord.z = i;
            vec4 offsets = texelFetch(OffsetTex, offsetCoord,0) * Radius * fragPosLightSpace.w;

            sc.xy = CurPixel + offsets.xy;
            sum += textureProj(shadowMap, sc);
            sc.xy = CurPixel + offsets.zw;
            sum += textureProj(shadowMap, sc);
        }
        shadow = sum / float(samplesDiv2 * 2.0f);
    }*/
}
void GetLVH(in vec4 LightPosition,out vec3 L,out vec3 V,out vec3 H)
{
   //fall vector
   L = normalize(LightPosition.xyz - fs_in.TangentFragPos * LightPosition.w);
   //view vector
   V = normalize(fs_in.ViewPos - fs_in.TangentFragPos);
   //half-way vector
   H = normalize(V + L);
}

float GetAttenuation(in vec4 LightPosition,in Light light)
{
    float distance    = length(LightPosition.xyz - fs_in.TangentFragPos) * LightPosition.w;
    return 1.0f / (light.clq.x + (light.clq.y + light.clq.z) * distance);
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

vec3 ImproveLight(Material mat,Texture tex)
{
    float shadow;
    vec3 N = normalize((2.0f * texture(tex.normal,fs_in.TexCoords).rgb) - 1.0f);  
    vec3 Lo = vec3(0.0);

    vec3 albedo = pow(texture(tex.diffuse, fs_in.TexCoords).rgb, vec3(gamma));
    float metallic = texture(tex.metallic_roughness, fs_in.TexCoords).b;
    float roughness = texture(tex.metallic_roughness, fs_in.TexCoords).g;
    float ao        = texture(tex.ao, fs_in.TexCoords).r;
    vec3 F0 = vec3(0.04); 
    F0      = mix(F0, albedo, metallic);

    vec3 L,V,H;

    for(uint i = 0;i < NumLights;++i)
    {
        GetLVH(fs_in.LightPositions[i],L,V,H);
  
        float attenuation = GetAttenuation(fs_in.LightPositions[i], light[i]);
        vec3 radiance     = light[i].ambient * attenuation;

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
        Lo += (kD * mat.ambient / PI + specular) * radiance * NdotL * ShadowCalculation(light[i].index);
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/gamma));
     
     return color;
}
/*
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
}*/