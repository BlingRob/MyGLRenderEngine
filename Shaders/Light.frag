#version 450 core 

out vec4 color;

struct Light
 {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

uniform Light light[1];

void main()
{
	color = vec4(light[0].diffuse,1.0f);
}