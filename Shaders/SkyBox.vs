#version 450 core
layout (location = 0) in vec3 position;

out vec3 TexCoords;

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
	TexCoords = position;
	vec4 pos = transform.PV * vec4(position,1.0f);
	gl_Position = pos.xyww;
};