#include "../Headers/Shader.h"

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath)
{
		// Link shaders
		GLint success;
		GLchar infoLog[512];
		GLuint fragmentShader, vertexShader,geometryShader;

		fragmentShader = CreateShader(fragmentPath, GL_FRAGMENT_SHADER);
		vertexShader = CreateShader(vertexPath, GL_VERTEX_SHADER);
		if (geometryPath)
			geometryShader = CreateShader(geometryPath,GL_GEOMETRY_SHADER);

		Program = glCreateProgram();
		glAttachShader(Program, vertexShader);
		glAttachShader(Program, fragmentShader);
		if (geometryPath)
			glAttachShader(Program, geometryShader);
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
		if (geometryPath)
			glDeleteShader(geometryShader);
}

void Shader::setTexture(std::string_view name, GLuint TextureIndx, GLuint PointBindIndex) const
{
	glBindTextureUnit(PointBindIndex, TextureIndx);
	glUniform1i(glGetUniformLocation(Program, name.data()), PointBindIndex);
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