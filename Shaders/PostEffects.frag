#version 450 core 

in vec3 TexCoords; 
uniform sampler2D scene;

out vec4 color;

void main()
{
	color = texture(scene, vec2(TexCoords));
}