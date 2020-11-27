#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 texCoord;

struct Transform
{
	mat4 model;
	mat4 view;
	mat4 projection;
};
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;


uniform Transform transform;
uniform mat3 NormalMatrix;
uniform mat4 lightSpaceMatrix;


void main()
{
    vs_out.FragPos = vec3(transform.model * vec4(position, 1.0));
    vs_out.Normal = transpose(inverse(mat3(transform.model))) * VertexNormal;
    vs_out.TexCoords = texCoord;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

	gl_Position = transform.projection * transform.view * transform.model * vec4(position,1.0f);
};