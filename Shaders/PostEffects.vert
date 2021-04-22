#version 450 core
layout (location = 0) in vec3 position;

out vec3 TexCoords;

void main()
{
	//TexCoords = position;
	//vec4 pos = transform.VP * vec4(position,1.0f);
	TexCoords = position;
};