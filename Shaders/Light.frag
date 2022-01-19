#version 450 core 

out vec4 color;

uniform vec3 Colour;

void main()
{
	color = vec4(Colour,1.0f);
}