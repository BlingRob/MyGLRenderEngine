#include "Shader.h"

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
		// 1. Получаем исходный код шейдера из filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// Удостоверимся, что ifstream объекты могут выкидывать исключения
		vShaderFile.exceptions(std::ifstream::failbit);
		fShaderFile.exceptions(std::ifstream::failbit);
		try
		{
			// Открываем файлы
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			if (!vShaderFile.is_open())
				throw("File with vertex shaders is not opened!");
			if(!fShaderFile.is_open())
				throw("File with fragment shaders is not opened!");
			std::stringstream vShaderStream, fShaderStream;
			// Считываем данные в потоки
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// Закрываем файлы
			vShaderFile.close();
			fShaderFile.close();
			// Преобразовываем потоки в массив GLchar
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();

		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		catch (const char* str) 
		{
			std::cerr << str << std::endl;
		}

		const GLchar* vss = vertexCode.c_str();
		const GLchar* fss = fragmentCode.c_str();

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vss, NULL);
		glCompileShader(vertexShader);
			// Check for compile time errors
		GLint success;
		GLchar infoLog[512];
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
		// Fragment shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fss, NULL);
		glCompileShader(fragmentShader);
		// Check for compile time errors
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		// Link shaders
		Program = glCreateProgram();
		glAttachShader(Program, vertexShader);
		glAttachShader(Program, fragmentShader);
		glLinkProgram(Program);
		// Check for linking errors
		glGetProgramiv(Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(Program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
}

void Shader::Use() { glUseProgram(Program); }