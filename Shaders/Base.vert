#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

const uint MAX_LIGHTS = 16;

struct Transform
{
	mat4 model;
	mat4 view;
	mat4 projection;
    mat4 PV;
}; 

uniform Transform transform;
uniform mat3 NormalMatrix;
uniform vec3 viewPos;
uniform uint NumLights;
uniform vec4 LightPositions[MAX_LIGHTS];

out VS_OUT {
    vec3 FragPos;
    vec3 ViewPos;
    vec2 TexCoords;
    vec3 TangentFragPos;
    
    vec4 LightPositions[MAX_LIGHTS];
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
    vs_out.ViewPos            = TBN * viewPos;
    vs_out.TangentFragPos     = TBN * vs_out.FragPos;

    for(uint i = 0;i < NumLights;++i)
    {
        vs_out.LightPositions[i].xyz = TBN * LightPositions[i].xyz; 
        vs_out.LightPositions[i].w = LightPositions[i].w; 
    }
    vs_out.TexCoords = texCoord;
	gl_Position = transform.PV * vec4(vs_out.FragPos,1.0f);

};