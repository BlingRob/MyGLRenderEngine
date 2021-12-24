#version 450 core
layout (location = 2) in vec2 position;

out vec2 TexCoords;
uniform float wAspect;
uniform float hAspect;

void main()
{
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
	TexCoords = ((position + 1.0f)/2.0f) * vec2(wAspect,hAspect);
	//TexCoords = vec2( ((position.x + 1.0f) / 2.0f) * wAspect,((position.y + 1.0f) / 2.0f) * hAspect);
};