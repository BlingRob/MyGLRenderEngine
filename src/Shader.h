#ifndef  SHADER_H
#define SHADER_H

#include "Headers.h"
#include "Entity.h"

class Shader:public Entity
{
	GLuint CreateShader(const GLchar* Path, GLenum types);
public:
	GLuint Program;
	Shader(const GLchar* vertexPath, const GLchar* framentPath);
	void Use();

	void setScal(std::string_view, const float) const;
	void setScal(std::string_view, const int) const;
	void setScal(std::string_view, const bool) const;

	void setVec(std::string_view, const glm::vec2&) const;
	void setVec(std::string_view, const glm::vec3&) const;
	void setVec(std::string_view, const glm::vec4&) const;

	void setMat(std::string_view, const glm::mat2&) const;
	void setMat(std::string_view, const glm::mat3&) const;
	void setMat(std::string_view, const glm::mat4&) const;

	void setSubroutine(GLenum ShaderType,std::string_view name);
};

#endif
