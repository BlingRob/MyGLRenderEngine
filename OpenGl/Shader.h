#ifndef  SHADER_H
#define SHADER_H

#include "Headers.h"

class Shader 
{
	GLuint CreateShader(const GLchar* Path, GLenum types);

public:
	GLuint Program;
	Shader(const GLchar* vertexPath, const GLchar* framentPath);
	void Use();

	void setScal(const std::string&, const float) const;
	void setScal(const std::string&, const int) const;
	void setScal(const std::string&, const bool) const;

	void setVec(const std::string&, const glm::vec2&) const;
	void setVec(const std::string&, const glm::vec3&) const;
	void setVec(const std::string&, const glm::vec4&) const;

	void setMat(const std::string&, const glm::mat2&) const;
	void setMat(const std::string&, const glm::mat3&) const;
	void setMat(const std::string&, const glm::mat4&) const;
	
};

#endif
