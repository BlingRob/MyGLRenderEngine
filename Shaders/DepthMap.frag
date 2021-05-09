#version 450 core
in vec4 FragPos;

uniform vec4 lightPos;
uniform float far_plane;

void main()
{
    // вычисление расстояния между фрагментом и источником 
    float lightDistance = length(FragPos.xyz - lightPos.xyz);
    
    // преобразование к интервалу [0, 1] посредством деления на far_plane
    lightDistance = lightDistance / far_plane;
    
    // запись результата в результирующую глубину фрагмента
    gl_FragDepth = lightDistance;
}