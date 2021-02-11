#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

struct Transform
{
	mat4 model;
	mat4 view;
	mat4 projection;
    mat4 MVP;
};
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    mat3 TBN;
} vs_out;

uniform Transform transform;
uniform mat3 NormalMatrix;
//uniform mat4 lightSpaceMatrix;

void main()
{
    vec3 N = normalize(NormalMatrix * VertexNormal);
    vec3 T = normalize(NormalMatrix * aTangent);
    vec3 B = normalize(NormalMatrix * aBitangent);
    //vec3 binormal = normalize(cross(norm,tang) * (-1.0f));

    vs_out.TBN = mat3(T,N,B);

    vs_out.FragPos = vec3(transform.model * vec4(position, 1.0));
    vs_out.Normal = NormalMatrix * VertexNormal;
    vs_out.TexCoords = texCoord;
    //vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

	gl_Position = transform.MVP * vec4(position,1.0f);

};