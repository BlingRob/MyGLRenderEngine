#ifndef SHADER_H
#define SHADER_H
#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Entity.h"
#include "gl/gl.h"

enum class Shader_t:uint32_t { Vertex = GL_VERTEX_SHADER, Fragment = GL_FRAGMENT_SHADER, Tess_Contr = GL_TESS_CONTROL_SHADER, Geometry = GL_GEOMETRY_SHADER, 
	Compute = GL_COMPUTE_SHADER };

class Shader:public Entity
{
public:
	GLuint Program;
	Shader(std::string_view VertexSource, std::string_view fragmentSource, std::string_view geometrySource = "");
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

private:
	void CreateProgramm(const std::vector<std::uint64_t>& shaders);
	GLuint CreateShader(const char* source, Shader_t type);
};

class ShadersBank
{
public:
	static const std::shared_ptr<Shader> GetShader(std::string_view name);
	static void AddShader(std::shared_ptr<Shader> sh);
	static inline std::size_t Size() { return Shaders.size(); };
	static inline void Clear() { Shaders.clear(); }
private:
	static inline std::map<std::size_t, std::shared_ptr<Shader>> Shaders;
};
#endif
