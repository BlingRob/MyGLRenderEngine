#version 450 core
layout (location = 0) in vec3 position;

struct Transform
{
	mat4 model;
	mat4 view;
	mat4 projection;
    mat4 PV;
};

uniform Transform transform;

void main()
{
	//gl_Position = transform.PV * transform.model * vec4(position,1.0f);
	gl_Position = transform.model * vec4(position,1.0f);
};