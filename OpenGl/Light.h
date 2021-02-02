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

class AmbientLight:public Light 
{
	glm::vec3 direction;

public:
	AmbientLight(float c = 0.0f, float l = 0.0f, float q = 0.0f, glm::vec3 a = glm::vec3(0.0f),
		glm::vec3 d = glm::vec3(0.0f), glm::vec3 s = glm::vec3(0.0f), glm::vec3 p = glm::vec3(0.0f), glm::vec3 dir = glm::vec3(0.0f)) :
		Light(c, l, q, a, d, s), direction(dir) {}
	virtual void SendToShader(std::shared_ptr<Shader> shader);
	glm::vec3 ChangeDirection(glm::vec3);
};

class PointLight : public Light
{
	glm::vec3 position;
	public:
	PointLight(float c = 0.0f, float l = 0.0f, float q = 0.0f, glm::vec3 a = glm::vec3(0.0f), glm::vec3 d = glm::vec3(0.0f), glm::vec3 s = glm::vec3(0.0f), glm::vec3 p = glm::vec3(0.0f)):
		Light(c,l,q,a,d,s),position(p){}
	/*Return old pos, set new pos*/
	virtual glm::vec3 SetPos(glm::vec3 p);
	/*Return current pos*/
	virtual glm::vec3 GetPos() const;
	virtual void SendToShader(std::shared_ptr<Shader> shader);

};

class SpotLight: public AmbientLight
{
	glm::vec3 direction;
	float Theta; //angel of big cone
	float Alpha; //angel of small cone
	public:
		SpotLight(float c = 0.0f, float l = 0.0f, float q = 0.0f, glm::vec3 a = glm::vec3(0.0f),
			glm::vec3 d = glm::vec3(0.0f), glm::vec3 s = glm::vec3(0.0f), glm::vec3 p = glm::vec3(0.0f),glm::vec3 dir = glm::vec3(0.0f),
			float BigAngel = 60, float SmallAngel = 30):AmbientLight(c, l, q, a, d, s, p, dir),Theta(BigAngel),Alpha(SmallAngel){}

	void SendToShader(std::shared_ptr<Shader> shader);
	std::pair<float, float> GetAngels();
	std::pair<float, float> GetAngels(std::pair<float, float> NewAngels);
};
