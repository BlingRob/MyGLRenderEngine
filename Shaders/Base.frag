#version 450 core

const float PI = 3.14159265359;
const float gamma = 2.2;
const uint MAX_LIGHTS_ONE_TYPE = 5;
const uint NUM_TYPES = 3;
const uint MAX_LIGHTS = NUM_TYPES * MAX_LIGHTS_ONE_TYPE;
const float frustumSize = 1;
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

    vec4 LightPositions;
    mat4 ShadowMatrix;
};  

struct Texture
{
    layout (binding = 0) sampler2D diffuse;
    layout (binding = 1) sampler2D normal;
    layout (binding = 2) sampler2D specular;
    layout (binding = 3) sampler2D emissive;
    layout (binding = 4) sampler2D height;
    layout (binding = 5) sampler2D metallic_roughness;
    //sampler2D roughness;
    layout (binding = 6) sampler2D ao;
};

uniform Material mat;
uniform Texture tex;

uniform uint NumLights;
uniform vec3 viewPos;
uniform Light light[MAX_LIGHTS];
//uniform vec4 LightPositions[MAX_LIGHTS];

layout (binding = 10) uniform samplerCubeArray PointShadowMaps;
/////////////////////////////
//Direction or spot lights uniforms
const uint NUM_SPOT_DIR_LIGHTS = 2 * MAX_LIGHTS_ONE_TYPE;
//uniform mat4 ShadowMatrix[NUM_SPOT_DIR_LIGHTS];
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
   // vec3 TangentViewPos;
    vec2 TexCoords;
   // vec3 TangentFragPos;
  //  vec4 TangentLightPositions[MAX_LIGHTS];
    vec3 Normal;
    vec4 ShadowCoords[NUM_SPOT_DIR_LIGHTS];
} fs_in;

vec3 getNormalFromMap();
void GetLVH(in vec4 LightPosition,out vec3 L,out vec3 V,out vec3 H);
float GetAttenuation(in vec4 LightPosition,in Light light);

vec3 PhongLight(Light,Material,Texture);
vec3 ImproveLight(Material,Texture);

float DistributionGGX(vec3, vec3, float);
float GeometrySchlickGGX(float, float);
float GeometrySmith(vec3, vec3, vec3, float);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

float PointShadowCalculation(uint);
float ShadowCalculationDirect(uint, vec4);

float PCSS_ShadowCalculationDirect(uint, vec4);
float PCSS_PointShadowCalculation(uint);

void main()
{ 
    //FragColor = vec4(texture(tex[0].normal, fs_in.TexCoords).rgb,0.0f);
    //FragColor = vec4(vec3(textureProj(shadowMap,fs_in.FragPosLightSpace)),0.0f);
    //FragColor = vec4(vec3(ShadowCalculation(0)),1.0f);
    FragColor = vec4(ImproveLight(mat, tex),1.0f);
    //FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));


    ////FragColor = vec4(fs_in.FragPos,1.0f);
    /*FragColor = vec4(0.0f);
    for(int i = 0;i < 1;++i)
        if(light[i].LightPositions.w == 1)
            FragColor += vec4(PointShadowCalculation(i));///point
       else
            FragColor += vec4(ShadowCalculationDirect(i, fs_in.ShadowCoords[i]));///spot or dir*/

    //FragColor = vec4(vec3(PointShadowCalculation(0)),1.0f);
    //vec3 fragToLight0 = fs_in.FragPos - vec3(LightPositions[0]); 
    //vec3 fragToLight1 = fs_in.FragPos - vec3(LightPositions[1]); 
    //FragColor += texture(shadowMaps, vec4(fragToLight1, 1.0f));
    //FragColor = vec4(1.0f - 1.0/pow(length(fs_in.FragPos - vec3(LightPositions[1])) + 0.0001,0.1f));
    //FragColor += vec4(1.0f - 1.0/(length(fs_in.FragPos - vec3(LightPositions[1]))+ 0.0001));
}

float ShadowCalculationDirect(uint index, vec4 fragPosLightSpace)
{
    fragPosLightSpace /= fragPosLightSpace.w;
    float closestDepth = texture(DirLightShadowMaps,vec3(fragPosLightSpace.xy,light[index].index)).r;
    float currentDepth = fragPosLightSpace.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 0.0 : 1.0;

    //closestDepth += textureOffset(DirLightShadowMaps, vec3(fragPosLightSpace.xy, light[index].index),ivec2(-1,-1)).r;
    //closestDepth += textureOffset(DirLightShadowMaps, vec3(fragPosLightSpace.xy, light[index].index),ivec2(-1,1)).r;
    //closestDepth += textureOffset(DirLightShadowMaps, vec3(fragPosLightSpace.xy, light[index].index),ivec2(1,-1)).r;
    //closestDepth += textureOffset(DirLightShadowMaps, vec3(fragPosLightSpace.xy, light[index].index),ivec2(1,1)).r;

    return shadow;// * 0.2;
    /*
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

float PointShadowCalculation(uint index)
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
    float shadow = currentDepth - bias  > closestDepth ? 0.0 : 1.0;
    return shadow;*/  
    vec3 fragToLight = fs_in.FragPos - vec3(light[index].LightPositions); 
    //float closestDepth = texture(shadowMaps, vec4(fragToLight, index)).r;
    //closestDepth *= far_plane; 
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias   = 0.005;
    int samples  = 20;
    float viewDistance = length(viewPos - fs_in.FragPos);
    //float diskRadius = 1.0f;
    //float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0f;

    currentDepth /= far_plane;
    float Dbl = texture(PointShadowMaps, vec4(fragToLight, index)).r;
    float Drec = abs(currentDepth - Dbl);
    float diskRadius = abs((Drec / Dbl) - 1.0f);
    //return texture(PointShadowMaps, vec4(fragToLight, light[index].index)).r;
    //float diskRadius = (1.0 + currentDepth) / 20.0;
    
    //float diskRadius = abs(length(fs_in.FragPos - vec3(light[index].LightPositions)) / length(far_plane - vec3(light[index].LightPositions)) );
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(PointShadowMaps, vec4(fragToLight + sampleOffsetDirections[i] * diskRadius, index)).r;
        //diskRadius = (1.0 + closestDepth) / 25.0;
       // closestDepth *= far_plane;   // обратное преобразование из диапазона [0;1]
        if(currentDepth - bias > closestDepth)
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
    return 1.0f - shadow;
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
/*
float PCSS_ShadowCalculationDirect(uint index, vec4 fragPosLightSpace)
{
    uint numBlockerSearchSamples = 2;
    float blockerDistance;
    int blockers = 0;
	float avgBlockerDistance = 0;
	float searchWidth = 1.0f * (fragPosLightSpace.z - NEAR) / eyePosition.z;

	for (int i = 0; i < numBlockerSearchSamples; ++i)
	{
		float z = texture(shadowMap, shadowCoords.xy + RandomDirection(distribution0, i / float(numBlockerSearchSamples)) * searchWidth).r;
		if (z < (shadowCoords.z - 0.005))
		{
			blockers++;
			avgBlockerDistance += z;
		}
	}

	blockerDistance = avgBlockerDistance / blockers;

}
float PCSS_PointShadowCalculation(uint index)
{
    mat4 lightView = mat4(1,0,0,0, 
		0,1,0,0, 
		0,0,1,0, 
		-light[index].LightPositions.x,-light[index].LightPositions.y,-light[index].LightPositions.z, 1);
	vec3 positionLightSpace = (lightView * invView * vec4(vCameraPosition, 1)).xyz;

    vec3 absPos = abs(positionLightSpace);
	float z = -max(absPos.x, max(absPos.y, absPos.z));
	vec4 clip = lightProjection * vec4(0.0, 0.0, z, 1.0);
	float receiverDistance =  (clip.z / clip.w) * 0.5 + 0.5;

	float z = texture(PointShadowMaps, vec4(positionLightSpace, index)).r;
	return (z < (receiverDistance - 0.005)) ? 0 : 1;

}*/
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(tex.normal, fs_in.TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.FragPos);
    vec3 Q2  = dFdy(fs_in.FragPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

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

vec3 ImproveLight(Material mat,Texture tex)
{
    float shadow,attenuation;
    //vec3 N = normalize((2.0f * texture(tex.normal,fs_in.TexCoords).rgb) - 1.0f); 
    vec3 N = getNormalFromMap(); 
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
        if(light[i].LightPositions.w == 1)
        {
            shadow = PointShadowCalculation(i);///point
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
        Lo += (kD * mat.ambient / PI + specular) * radiance * NdotL * shadow;
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