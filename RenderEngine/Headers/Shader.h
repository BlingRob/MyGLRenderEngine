#ifndef  SHADER_H
#define SHADER_H
#include <string_view>
#include <string>
#include <fstream>
#include <sstream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Entity.h"
#include "gl/gl.h"

class Shader:public Entity
{
	GLuint CreateShader(const GLchar* Path, GLenum types);
public:
	GLuint Program;
	Shader(const GLchar* vertexPath, const GLchar* framentPath,const GLchar* geometryPath = nullptr);
	void Use();

	void setScal(std::string_view, const float) const;
	void setScal(std::string_view, const int) const;
	void setScal(std::string_view, const bool) const;
	void setScal(std::string_view, const uint64_t) const;
	void setScal(std::string_view, const GLuint) const;

	void setVec(std::string_view, const glm::vec2&) const;
	void setVec(std::string_view, const glm::vec3&) const;
	void setVec(std::string_view, const glm::vec4&) const;

	void setMat(std::string_view, const glm::mat2&) const;
	void setMat(std::string_view, const glm::mat3&) const;
	void setMat(std::string_view, const glm::mat4&) const;
	void setMats(std::string_view, const GLfloat*, GLsizei) const;

	void setSubroutine(std::string_view name, GLenum ShaderType) const;
	void setTexture(std::string_view name, GLuint TextureIndx,GLuint PointBindIndex) const;
};

#endif