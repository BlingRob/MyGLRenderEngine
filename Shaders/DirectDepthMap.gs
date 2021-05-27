#version 450 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform int Index;

out vec4 FragPos; 

void main()
{
    gl_Layer = Index;
    FragPos = gl_in[0].gl_Position;
    FragPos = gl_in[1].gl_Position;
    FragPos = gl_in[2].gl_Position;
    EmitVertex();
    EndPrimitive();
}