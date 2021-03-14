#include "Shader.h"

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
		// Link shaders
		GLint success;
		GLchar infoLog[512];
		GLuint fragmentShader, vertexShader;

		fragmentShader = CreateShader(fragmentPath, GL_FRAGMENT_SHADER);
		vertexShader = CreateShader(vertexPath, GL_VERTEX_SHADER);

		Program = glCreateProgram();
		glAttachShader(Program, vertexShader);
		glAttachShader(Program, fragmentShader);
		glLinkProgram(Program);
		// Check for linking errors
		glGetProgramiv(Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(Program, 512, NULL, infoLog);
			throw(std::string("ERROR::SHADER::PROGRAM::LINKING_FAILED\n") + infoLog);
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
}

void Shader::Use() { glUseProgram(Program); }

GLuint Shader::CreateShader(const GLchar* Path, GLenum types)
{
	// 1. Получаем исходный код шейдера из filePath
	std::string ShaderCode;
	std::ifstream ShaderFile;
	std::stringstream ShaderStream;
	// Удостоверимся, что ifstream объекты могут выкидывать исключения
	ShaderFile.exceptions(std::ifstream::failbit);

	// Открываем файлы
	ShaderFile.open(Path);
	if (!ShaderFile.is_open())
		throw("File with shaders is not opened!");
	// Считываем данные в потоки
	ShaderStream << ShaderFile.rdbuf();
	// Закрываем файлы
	ShaderFile.close();
	ShaderCode = ShaderStream.str();
	const GLchar* CShaderCode = ShaderCode.c_str();

	GLuint Shader = glCreateShader(types);
	glShaderSource(Shader, 1, &CShaderCode, NULL);
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

void Shader::setScal(const std::string& name, const float num) const
{
	glUniform1f(glGetUniformLocation(Program, name.c_str()), num);
}
void Shader::setScal(const std::string& name, const int num) const
{
	glUniform1i(glGetUniformLocation(Program, name.c_str()), num);
}
void Shader::setScal(const std::string& name, const bool num) const
{
	glUniform1i(glGetUniformLocation(Program, name.c_str()), num);
}

void Shader::setVec(const std::string& name,const glm::vec2& vec) const 
{
	glUniform2fv(glGetUniformLocation(Program, name.c_str()), 1, glm::value_ptr(vec));
}
void Shader::setVec(const std::string& name, const glm::vec3& vec) const
{
	glUniform3fv(glGetUniformLocation(Program, name.c_str()), 1, glm::value_ptr(vec));
}
void Shader::setVec(const std::string& name, const glm::vec4& vec) const
{
	glUniform4fv(glGetUniformLocation(Program, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::setMat(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(Program, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setMat(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(Program, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setMat(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(Program, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}