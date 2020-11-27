#pragma once
#include "Headers.h"
#include "Shader.h"

class Light
{
	float constant;
	float linear;
	float quadratic;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	public:
		Light(float c = 0.0f, float l = 0.0f, float q = 0.0f, glm::vec3 a = glm::vec3(0.0f), glm::vec3 d = glm::vec3(0.0f), glm::vec3 s = glm::vec3(0.0f))
		{
			constant = c;
			linear = l;
			quadratic = q;
			ambient = a;
			diffuse = d;
			specular = s;
		}
		virtual void SendToShader(std::shared_ptr<Shader> shader);
};

class PointLight : public Light
{
	glm::vec3 position;
	public:
	PointLight(float c = 0.0f, float l = 0.0f, float q = 0.0f, glm::vec3 a = glm::vec3(0.0f), glm::vec3 d = glm::vec3(0.0f), glm::vec3 s = glm::vec3(0.0f), glm::vec3 p = glm::vec3(0.0f)):
		Light(c,l,q,a,d,s)
	{
		position = p;
	}
	/*Return old pos, set new pos*/
	glm::vec3 SetPos(glm::vec3 p);
	/*Return current pos*/
	glm::vec3 GetPos() const;
	void SendToShader(std::shared_ptr<Shader> shader);

};

