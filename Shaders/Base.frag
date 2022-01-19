//Base lighting fragment shader
#version 450 core

const float PI = 3.14159265359;
const float gamma = 2.2;
const uint MAX_LIGHTS_ONE_TYPE = 5;
const uint NUM_TYPES = 3;
const uint MAX_LIGHTS = NUM_TYPES * MAX_LIGHTS_ONE_TYPE;
const float frustumSize = 1;

const vec3 sampleOffsetDirections[27] = vec3[]
(
   vec3( 0,  0,  0), vec3( 0,  0,  1),  vec3(0,  1,  0), vec3(0,  1,  1), 
   vec3( 1,  0, 0),  vec3( 1, 0, 1),    vec3(1, 1, 0),   vec3(1,  1, 1),
   vec3( 0,  0,  -1),vec3(0,  -1,  0), vec3(0,  -1,  -1), 
   vec3( -1,  0, 0), vec3( -1, 0, -1), vec3(-1, -1, 0),  vec3(-1,  -1, -1),
   vec3(0,  1,  -1), vec3(0,  -1,  1), 
   vec3( 1, 0, -1),  vec3( -1, 0, 1),vec3(1, -1, 0),vec3(-1, 1, 0),
   vec3(1, 1, -1),   vec3(1,  -1, 1),vec3(1,  -1, -1),
   vec3(-1, 1, 1),   vec3(-1,  1, -1),vec3(-1,  -1, 1)
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

    vec4 LightPositions;
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

uniform Material mat;
uniform Texture tex;

uniform uint NumLights;
uniform vec3 viewPos;
uniform Light light[MAX_LIGHTS];
uniform mat4 lightProjection;
layout (binding = 10) uniform samplerCubeArray PointShadowMaps;
/////////////////////////////
//Direction or spot lights uniforms
const uint NUM_SPOT_DIR_LIGHTS = 2 * MAX_LIGHTS_ONE_TYPE;
layout (binding = 11) uniform sampler2DArray DirLightShadowMaps;
//////////////////////////////
layout (binding = 12) uniform sampler3D OffsetTex;

uniform float Radius;
uniform vec3 OffsetTexSize; // (width, height, depth)
////////////////////////////////
uniform float far_plane;

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 TangentViewPos;
    vec2 TexCoords;
    vec3 TangentFragPos;
  //  vec4 TangentLightPositions[MAX_LIGHTS];
    vec3 Normal;
    vec4 ShadowCoords[NUM_SPOT_DIR_LIGHTS];
} fs_in;

vec3 getNormalFromMap(vec2 texCoord);
void GetLVH(in vec4 LightPosition,out vec3 L,out vec3 V,out vec3 H);
float GetAttenuation(in vec4 LightPosition,in Light light);

vec3 PhongLight(Light,Material,Texture);
vec3 ImproveLight(Material,Texture,vec2);
vec2 ParallaxMapping(vec2 texCoords,vec3 viewDir);

float DistributionGGX(vec3, vec3, float);
float GeometrySchlickGGX(float, float);
float GeometrySmith(vec3, vec3, vec3, float);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

float PointShadowCalculation(int,vec3);
float ShadowCalculationDirect(int, vec4);

float PCSS_ShadowCalculationDirect(uint, vec4);
float PCSS_PointLight(uint);

void main()
{ 
    vec3 viewDir   = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    //FragColor = vec4(texture(tex[0].normal, fs_in.TexCoords).rgb,0.0f);
    //FragColor = vec4(vec3(ShadowCalculation(0)),1.0f);
    FragColor = vec4(ImproveLight(mat, tex, fs_in.TexCoords),1.0f);//ParallaxMapping(fs_in.TexCoords,viewDir)
    //FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
	//FragColor = vec4(fs_in.FragPos.z);

    ////FragColor = vec4(fs_in.FragPos,1.0f);
	
    //vec3 fragToLight = fs_in.FragPos - vec3(light[2].LightPositions);
	//FragColor = texture(PointShadowMaps, vec4(fragToLight, light[2].index));
    
   ///////////////// float closestDepth = texture(DirLightShadowMaps,vec3(fs_in.ShadowCoords[0].xy / fs_in.ShadowCoords[0].w,0)).r;
   //////////////// float currentDepth = fs_in.ShadowCoords[0].z;
    // check whether current frag pos is in shadow
	////////////FragColor = vec4(currentDepth);
    //FragColor = vec4(currentDepth > closestDepth  ? 0.0 : 1.0);
	
	//FragColor = texture(DirLightShadowMaps,vec3(fs_in.ShadowCoords[0].xy / fs_in.ShadowCoords[0].w,0));
	//FragColor = vec4(ShadowCalculationDirect(0, fs_in.ShadowCoords[0]));
    /*vec3 N = fs_in.Normal;
    //FragColor = vec4(0.0f);
	//vec3 N = getNormalFromMap(); 
    //for(int i = 0;i < NumLights;++i)
    //   if(light[i].LightPositions.w == 0)
     //       FragColor += vec4(PointShadowCalculation(i, N));///point
       //else
    //        FragColor = vec4(ShadowCalculationDirect(i, fs_in.ShadowCoords[light[i].index]));*////spot or dir

    //FragColor = vec4(vec3(PointShadowCalculation(0)),1.0f);
    //FragColor += texture(shadowMaps, vec4(fragToLight1, 1.0f));
}

float ShadowCalculationDirect(int index, vec4 fragPosLightSpace)
{
    fragPosLightSpace /= fragPosLightSpace.w;
    //float closestDepth = texture(DirLightShadowMaps,vec3(fragPosLightSpace.xy,light[index].index)).r;
    //float currentDepth = fragPosLightSpace.z;
    // check whether current frag pos is in shadow
    //float shadow = currentDepth > closestDepth  ? 0.0 : 1.0;

    //closestDepth += textureOffset(DirLightShadowMaps, vec3(fragPosLightSpace.xy, light[index].index),ivec2(-1,-1)).r;
    //closestDepth += textureOffset(DirLightShadowMaps, vec3(fragPosLightSpace.xy, light[index].index),ivec2(-1,1)).r;
    //closestDepth += textureOffset(DirLightShadowMaps, vec3(fragPosLightSpace.xy, light[index].index),ivec2(1,-1)).r;
    //closestDepth += textureOffset(DirLightShadowMaps, vec3(fragPosLightSpace.xy, light[index].index),ivec2(1,1)).r;
    
    ivec3 offsetCoord;
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
        sum += texture(DirLightShadowMaps,vec3(sc.xy,light[index].index)).r;
        sc.xy = CurPixel + offsets.zw;
        sum += texture(DirLightShadowMaps,vec3(sc.xy,light[index].index)).r;
    }
    float shadow = sum / 8.0;
    
    if( shadow != 1.0 && shadow != 0.0 ) 
    {
        for( int i = 4; i < samplesDiv2; ++i )
         {
            offsetCoord.z = i;
            vec4 offsets = texelFetch(OffsetTex, offsetCoord,0) * Radius * fragPosLightSpace.w;

            sc.xy = CurPixel + offsets.xy;
            sum += texture(DirLightShadowMaps,vec3(sc.xy,light[index].index)).r;
            sc.xy = CurPixel + offsets.zw;
            sum += texture(DirLightShadowMaps,vec3(sc.xy,light[index].index)).r;
        }
        shadow = sum / float(samplesDiv2 * 2.0f);
    }
    return shadow;
}

float PointShadowCalculation(int index,vec3 normal)
{
    vec3 fragToLight = fs_in.FragPos - vec3(light[index].LightPositions);
    if(min(dot(fragToLight,normal),0.0f) == 0.0f)
        return 0.0f; 
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias   = 0.005;
    int samples  = 27;
    float viewDistance = length(viewPos - fs_in.FragPos);

    currentDepth /= far_plane;
    float Dbl = texture(PointShadowMaps, vec4(fragToLight, light[index].index)).r;
    float Drec = abs(currentDepth - Dbl);
    
    float diskRadius = 0.05;

    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(PointShadowMaps, vec4(fragToLight + sampleOffsetDirections[i] * smoothstep(0.0f, 15.0f, abs(1.0f - currentDepth / (Dbl + 0.0001))), light[index].index)).r;
        if(currentDepth - bias < closestDepth)
            shadow += 1.0;
    }
    /*
    if(shadow == 4.0f)
        return 0.0f;
    for(int i = 8; i < samples; ++i)
    {
        float closestDepth = texture(PointShadowMaps, vec4(fragToLight + sampleOffsetDirections[i] * diskRadius, index)).r;
        //diskRadius = (1.0 + closestDepth) / 25.0;
       // closestDepth *= far_plane;   // обратное преобразование из диапазона [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }*/

    shadow /= float(samples);
    return shadow;
    //return diskRadius * 100.0f;
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
}

vec3 getNormalFromMap(vec2 texCoord)
{
    vec3 tangentNormal = texture(tex.normal, texCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.FragPos);
    vec3 Q2  = dFdy(fs_in.FragPos);
    vec2 st1 = dFdx(texCoord);//&
    vec2 st2 = dFdy(texCoord);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void GetLVH(in vec4 LightPosition,out vec3 L,out vec3 V,out vec3 H)
{
   //fall vector
   L = normalize(LightPosition.xyz - fs_in.FragPos * LightPosition.w);
   //view vector
   V = normalize(viewPos - fs_in.FragPos);
   //half-way vector
   H = normalize(V + L);
}

float GetAttenuation(in vec4 LightPosition,in Light light)
{
    float distance    = length(LightPosition.xyz - fs_in.FragPos) * LightPosition.w;
    return 1.0f / (light.clq.x + (light.clq.y + light.clq.z) * distance);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelCookTorrance(float cosTheta, vec3 F0)
{
    vec3 sqrtF0 = sqrt(F0);
    vec3 New = (1 + sqrtF0)/(1 - sqrtF0);
    float c = cosTheta;
    vec3 g = sqrt(New * New + c * c - 1);

    vec3 gPlusC = g + c;
    vec3 gMinusC = g - c;

    return 0.5 * pow(gMinusC / gPlusC, vec3(2.0f)) * (1 + pow((gPlusC * c - 1) / (gMinusC * c + 1), vec3(2.0f)));
}

float DistributionBeckman(vec3 N, vec3 H, float roughness)
{
    float a     = pow(roughness,4);
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float NdotH4 = NdotH2 * NdotH2;

    float nom    = exp((NdotH2 - 1)/(a * NdotH2));
    float denom = PI * a * NdotH4;

     return nom / denom;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a     = pow(roughness,4);
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
	
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

float GeometryCookTorrance(vec3 N, vec3 V, vec3 L, float roughness)
{
    vec3 H = (V + L) / 2.0f; 
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float k = 2 * max(dot(N, H), 0.0) / max(dot(V, H), 0.0);

    return min(min(1.0f,k * NdotV),k * NdotL);
}

vec3 ImproveLight(Material mat,Texture tex,vec2 texCoord)
{
    float shadow,attenuation;
    //vec3 N = normalize((2.0f * texture(tex.normal,fs_in.TexCoords).rgb) - 1.0f); 
    vec3 N = getNormalFromMap(texCoord); 
    vec3 Lo = vec3(0.0);

    vec3 albedo = pow(texture(tex.diffuse, texCoord).rgb, vec3(gamma));
    float metallic = texture(tex.metallic_roughness, texCoord).b;
    float roughness = texture(tex.metallic_roughness, texCoord).g;
    float ao        = texture(tex.ao, texCoord).r;
    vec3 F0 = vec3(0.04); 
    F0      = mix(F0, albedo, metallic);

    vec3 L,V,H;

    for(int i = 0;i < NumLights;++i)
    {
        if(light[i].LightPositions.w == 1)
        {
            shadow = PointShadowCalculation(i,fs_in.Normal);///point
            GetLVH(light[i].LightPositions,L,V,H);
            attenuation = GetAttenuation(light[i].LightPositions, light[i]);
        }
        else
        {
            GetLVH(-light[i].LightPositions,L,V,H);
            shadow = ShadowCalculationDirect(i, fs_in.ShadowCoords[i]);///spot or dir
            attenuation = 1;
        }

        vec3 radiance = light[i].ambient * attenuation;

        // Cook-Torrance BRDF
        //float NDF = DistributionBeckman(N, H, roughness);       
        float G   = GeometryCookTorrance(N, V, L, roughness);
        vec3 F  = fresnelCookTorrance(max(dot(H, V), 0.0), F0); 
        float NDF = DistributionGGX(N, H, roughness);       
        //float G   = GeometrySmith(N, V, L, roughness);
        //vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0); 

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular     = numerator / denominator; 
         //Fresnel coefficient
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
  
        kD *= 1.0 - metallic;
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * mat.ambient / PI + specular) * radiance * NdotL * shadow;
    }

    vec3 ambient = vec3(0.03) * albedo;// * ao;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/gamma));
     
     return color;
}

vec2 ParallaxMapping(vec2 texCoords,vec3 viewDir)
{
    float height = texture(tex.height, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height * 0.1);
    return texCoords - p;

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
