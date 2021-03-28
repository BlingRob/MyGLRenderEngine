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
    mat4 VP;
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

out VS_OUT {
    vec3 ViewDir;
    //vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    PointLight light;
} vs_out;

uniform Transform transform;
uniform mat3 NormalMatrix;
uniform vec3 viewPos;
uniform PointLight light;
//uniform mat4 lightSpaceMatrix;

void main()
{
    vs_out.light = light;

    vec3 N = normalize(NormalMatrix * VertexNormal);
    vec3 T = normalize(NormalMatrix * aTangent);
    vec3 B = normalize(NormalMatrix * aBitangent);
    T = normalize(T - dot(T, N) * N);

    mat3 TBN = transpose(mat3(T,B,N));

    vec3 Pos =  vec3(transform.model * vec4(position, 1.0));

    vs_out.light.LightDir =  normalize(TBN * (light.position.xyz - Pos));
    //vs_out.ViewDir  = normalize(TBN * (viewPos - Pos));
    vs_out.ViewDir  = normalize(TBN * (-Pos));
    //vs_out.FragPos =  TBN * vec3(transform.model * vec4(position, 1.0));

    //vs_out.Normal = NormalMatrix * VertexNormal;
    vs_out.TexCoords = texCoord;
    //vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

	gl_Position = (transform.VP * transform.model) * vec4(position,1.0f);

};