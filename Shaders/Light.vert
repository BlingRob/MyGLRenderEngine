#version 450 core
layout (location = 0) in vec3 position;

struct Transform
{
	mat4 model;
	mat4 view;
	mat4 projection;
    mat4 VP;
};
uniform Transform transform;

void main()
{
	gl_Position = transform.VP * transform.model * vec4(position,1.0f);
};