#include "../Headers/Shader.h"

Shader::Shader(std::string_view VertexSource, std::string_view fragmentSource, std::string_view geometrySource)
{
		// Link shaders
		GLint success;
		GLchar infoLog[512];
		std::vector<std::uint64_t> shaders;

		if(VertexSource.empty() || fragmentSource.empty())
			throw(std::string("ERROR::SHADER::THERE AREN'T VERTEX OR FRAGEMNT SHADERS\n"));
		shaders.push_back(CreateShader(fragmentSource.data(), Shader_t::Fragment));
		shaders.push_back(CreateShader(VertexSource.data(), Shader_t::Vertex));
		if (!geometrySource.empty())
			shaders.push_back(CreateShader(geometrySource.data(), Shader_t::Geometry));

		CreateProgramm(shaders);

		// Check for linking errors
		glGetProgramiv(Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(Program, 512, NULL, infoLog);
			throw(std::string("ERROR::SHADER::PROGRAM::LINKING_FAILED\n") + VertexSource.substr(0, VertexSource.find_first_of('\n')).data() + fragmentSource.substr(0, fragmentSource.find_first_of('\n')).data() + infoLog);
		}

		for (const auto& sh : shaders)
			glDeleteShader(sh);
}

void Shader::Use() { glUseProgram(Program); }

GLuint Shader::CreateShader(const char* source, Shader_t type)
{
	GLuint Shader = glCreateShader(GLenum(type));
	glShaderSource(Shader, 1, &source, NULL);
	glCompileShader(Shader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(Shader, 512, NULL, infoLog);
		throw(std::string("ERROR::SHADER::COMPILATION_FAILED\n") + infoLog);
	}
	return Shader;
}

void Shader::CreateProgramm(const std::vector<std::uint64_t>& shaders)
{
	Program = glCreateProgram();
	for (const auto& sh : shaders)
		glAttachShader(Program, sh);

	glLinkProgram(Program);
}

void Shader::setTexture(std::string_view name, GLuint TextureIndx, GLuint PointBindIndex) const
{
	glBindTextureUnit(PointBindIndex, TextureIndx);
	glUniform1i(glGetUniformLocation(Program, name.data()), PointBindIndex);
}

void Shader::setScal(std::string_view name, const float num) const
{
	glUniform1f(glGetUniformLocation(Program, name.data()), num);
}
void Shader::setScal(std::string_view name, const int num) const
{
	glUniform1i(glGetUniformLocation(Program, name.data()), num);
}
void Shader::setScal(std::string_view name, const bool num) const
{
	glUniform1i(glGetUniformLocation(Program, name.data()), num);
}
void Shader::setScal(std::string_view name, const uint64_t num) const
{
	glUniform1ui(glGetUniformLocation(Program, name.data()), num);
}
void Shader::setScal(std::string_view name, const GLuint num) const
{
	glUniform1ui(glGetUniformLocation(Program, name.data()), num);
}

void Shader::setVec(std::string_view name,const glm::vec2& vec) const
{
	glUniform2fv(glGetUniformLocation(Program, name.data()), 1, glm::value_ptr(vec));
}
void Shader::setVec(std::string_view name, const glm::vec3& vec) const
{
	glUniform3fv(glGetUniformLocation(Program, name.data()), 1, glm::value_ptr(vec));
}
void Shader::setVec(std::string_view name, const glm::vec4& vec) const
{
	glUniform4fv(glGetUniformLocation(Program, name.data()), 1, glm::value_ptr(vec));
}

void Shader::setMat(std::string_view name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(Program, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setMat(std::string_view name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(Program, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setMat(std::string_view name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(Program, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setSubroutine(std::string_view name, GLenum ShaderType) const
{
	GLuint Index = glGetSubroutineIndex(Program, ShaderType, name.data());
	glUniformSubroutinesuiv(ShaderType, 1, &Index);
}

void Shader::setMats(std::string_view name, const GLfloat* ArrayOfMatrices, GLsizei n) const
{
	glUniformMatrix4fv(glGetUniformLocation(Program, name.data()), n, GL_FALSE, ArrayOfMatrices);
}

const std::shared_ptr<Shader> ShadersBank::GetShader(std::string_view name)
{
	if (auto iter = Shaders.find(std::hash<std::string_view>{}(name)); iter != Shaders.end())
		return iter->second;
	return nullptr;
}

void ShadersBank::AddShader(std::shared_ptr<Shader> sh)
{
	Shaders[std::hash<std::string_view>{}(sh->GetName())] = sh;
}
