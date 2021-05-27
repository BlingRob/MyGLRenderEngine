#version 450 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];
uniform int Index;
// переменная FragPos вычисляется в геометрическом шейдере 
// и выдается для каждого вызова EmitVertex()
out vec4 FragPos; 

void main()
{
    for(int face = 6 * Index; face < 6 * (Index + 1); ++face)
    {
        // встроенная переменная, определяющая в какую 
        // грань кубической карты идет рендер
        gl_Layer = face; 
        for(int i = 0; i < 3; ++i) // цикл по всем вершинам треугольника
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face - 6 * Index] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}