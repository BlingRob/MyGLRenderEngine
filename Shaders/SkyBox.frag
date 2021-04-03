#version 450 core 

in vec3 TexCoords; 
uniform samplerCubeArray skybox;

out vec4 color;

void main()
{
	color = texture(skybox, vec4(TexCoords,0.0f));
}