#ifndef  SHADER_H
#define SHADER_H

#include "Headers.h"
#include "Entity.h"

class Shader:public Entity
{
	GLuint CreateShader(const GLchar* Path, GLenum types);
public:
	GLuint Program;
	Shader(const GLchar* vertexPath, const GLchar* framentPath,const GLchar* geometryPath);
	void Use();

	void setScal(std::string_view, const float) const;
	void setScal(std::string_view, const int) const;
	void setScal(std::string_view, const bool) const;
	void Shader::setScal(std::string_view, const uint64_t) const;

	void setVec(std::string_view, const glm::vec2&) const;
	void setVec(std::string_view, const glm::vec3&) const;
	void setVec(std::string_view, const glm::vec4&) const;

	void setMat(std::string_view, const glm::mat2&) const;
	void setMat(std::string_view, const glm::mat3&) const;
	void setMat(std::string_view, const glm::mat4&) const;
	void setMats(std::string_view, const GLfloat*, GLsizei) const;

	void setSubroutine(std::string_view name, GLenum ShaderType);
};

#endif
