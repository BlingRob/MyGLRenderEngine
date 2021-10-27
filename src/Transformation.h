#pragma once
#include "Headers.h"
#include "Shader.h"
#include "Camera.h"

struct Matrices 
{
	Matrices() 
	{
		Projection = std::make_shared<glm::mat4>(1.0f);
		View = std::make_shared<glm::mat4>(1.0f);
	}
	std::shared_ptr<glm::mat4> Projection,
		View;
	void SendToShader(const Shader& shader);
};

struct Position_Controller:public Matrices
{
	Position_Controller()
	{
		dt = std::make_shared<float>(0.0f);
		cam = std::make_shared<Camera>(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	}

	std::shared_ptr<Camera> cam;
	//delta time
	std::shared_ptr<float> dt;
	
};


class Transformation
{
	public:
		Transformation(std::shared_ptr<glm::mat4> mod = std::make_shared<glm::mat4>(1.0f)):Model(mod), NormalMatrix(glm::mat3(glm::transpose(glm::inverse(*Model)))){}
		void SendToShader(const Shader& shader);

		void Set(const std::shared_ptr <glm::mat4>);
		std::shared_ptr <glm::mat4> Get() const;

		void Translate(const glm::vec3& trans);
		void Rotate(float alph, const glm::vec3& axes);
		void Scale(const glm::vec3& coefs);

	private:
		std::shared_ptr<glm::mat4> Model;
		glm::mat3 NormalMatrix;
		void updateNormal();
};

