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
    mat3 toObjectLocal;
} vs_out;

uniform Transform transform;
uniform mat3 NormalMatrix;
//uniform mat4 lightSpaceMatrix;

void main()
{
    vec3 norm = normalize(NormalMatrix * VertexNormal);
    vec3 tang = normalize(NormalMatrix * aTangent);
    vec3 binormal = normalize(cross(norm,tang) * (-1.0f));

    vs_out.toObjectLocal = mat3
    (
        tang.x,binormal.x,norm.x,
        tang.y,binormal.y,norm.y,
        tang.z,binormal.z,norm.z
    );

    vs_out.FragPos = vec3(transform.model * vec4(position, 1.0));
    vs_out.Normal = transpose(inverse(mat3(transform.model))) * VertexNormal;
    vs_out.TexCoords = texCoord;
    //vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

	gl_Position = transform.MVP * vec4(position,1.0f);

};