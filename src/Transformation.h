#pragma once
#include "Headers.h"
#include "Shader.h"

struct Matrices
{
	Matrices():View(std::make_shared<glm::mat4>(1.0f)),Projection(std::make_shared<glm::mat4>(1.0f)) {}
	std::shared_ptr<glm::mat4> View,Projection;
	void SendToShader(const Shader& shader);
};

class Transformation
{
	public:
		Transformation(std::shared_ptr<glm::mat4> mod = std::make_shared<glm::mat4>(1.0f)):Model(mod) {}
		void SendToShader(const Shader& shader);

		void Set(const std::shared_ptr <glm::mat4>);
		std::shared_ptr <glm::mat4> Get() const;

		void Translate(const glm::vec3& trans);
		void Rotate(float alph, const glm::vec3& axes);
		void Scale(const glm::vec3& coefs);

	private:
		std::shared_ptr<glm::mat4> Model;
};

