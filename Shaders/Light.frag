#version 450 core 

out vec4 color;

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

uniform PointLight light;

void main()
{
	color = vec4(light.diffuse,1.0f);
}