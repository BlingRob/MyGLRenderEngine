#ifndef  SHADER_H
#define SHADER_H

#include "Headers.h"

class Shader 
{
public:
	GLuint Program;
	Shader(const GLchar* vertexPath, const GLchar* framentPath);
	void Use();
};

#endif
