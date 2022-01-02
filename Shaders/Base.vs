//Base lighting vertex shader
#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

const uint MAX_LIGHTS_ONE_TYPE = 5;
const uint NUM_TYPES = 3;
const uint MAX_LIGHTS = NUM_TYPES * MAX_LIGHTS_ONE_TYPE;
const uint NUM_SPOT_DIR_LIGHTS = 2 * MAX_LIGHTS_ONE_TYPE;

struct Transform
{
	mat4 model;
	mat4 view;
	mat4 projection;
	mat4 PV;
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

uniform Light light[MAX_LIGHTS];
uniform Transform transform;
uniform mat3 NormalMatrix;
uniform vec3 viewPos;
uniform uint NumLights;
//uniform vec4 LightPositions[MAX_LIGHTS];
//uniform mat4 ShadowMatrix[NUM_SPOT_DIR_LIGHTS];

out VS_OUT {
    vec3 FragPos;
    vec3 TangentViewPos;
    vec2 TexCoords;
    vec3 TangentFragPos;
  //vec4 TangentLightPositions[MAX_LIGHTS];
    vec3 Normal;
    vec4 ShadowCoords[NUM_SPOT_DIR_LIGHTS];
} vs_out;

void main()
{
    vs_out.FragPos = vec3(transform.model * vec4(position,1.0f));

    vec3 N = normalize(NormalMatrix * VertexNormal);
    vec3 T = normalize(NormalMatrix * aTangent);
    vec3 B = normalize(NormalMatrix * aBitangent);
    //T = normalize(T - dot(T, N) * N);

    mat3 TBN = transpose(mat3(T,B,N));
    //translate coordinats in tangent space
    vs_out.TangentViewPos     = TBN * viewPos;
    vs_out.TangentFragPos     = TBN * vs_out.FragPos;
    
    vs_out.Normal = NormalMatrix * VertexNormal;   
    for(uint i = 0;i < NumLights;++i)
    {
        //vs_out.TangentLightPositions[i].xyz = TBN * light[i].LightPositions.xyz; 
        //vs_out.TangentLightPositions[i].w = light[i].LightPositions.w; 
        vs_out.ShadowCoords[i] = light[i].ShadowMatrix * vec4(vs_out.FragPos, 1.0);
    }
    vs_out.TexCoords = texCoord;
	gl_Position = transform.PV * vec4(vs_out.FragPos,1.0f);

};
