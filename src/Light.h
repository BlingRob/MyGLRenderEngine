#pragma once
#include "Headers.h"
#include "Shader.h"
#include "Transformation.h"

class BLight
{
	std::string name;
	float constant;
	float linear;
	float quadratic;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	public:
	BLight(float c = 0.0f, float l = 0.0f, float q = 0.0f, glm::vec3 a = glm::vec3(0.0f), glm::vec3 d = glm::vec3(0.0f), glm::vec3 s = glm::vec3(0.0f))
		{
			constant = c;
			linear = l;
			quadratic = q;
			ambient = a;
			diffuse = d;
			specular = s;
		}
	glm::vec3 GetAttenuation() const;
	glm::vec3 GetAmbient() const;
	glm::vec3 GetDiffuse() const;
	glm::vec3 GetSpecular() const;
	std::string GetName() const;

	void SetAttenuation(const glm::vec3&);
	void SetAmbient(const glm::vec3&);
	void SetDiffuse(const glm::vec3&);
	void SetSpecular(const glm::vec3&);
	void SetName(const std::string&);
	
	virtual void SendToShader(const Shader& shader);

};

class DirectionalLight:public virtual BLight
{
	glm::vec3 direction;

public:
	DirectionalLight(float c = 0.0f, float l = 0.0f, float q = 0.0f, glm::vec3 a = glm::vec3(0.0f),
		glm::vec3 d = glm::vec3(0.0f), glm::vec3 s = glm::vec3(0.0f), glm::vec3 p = glm::vec3(0.0f), glm::vec3 dir = glm::vec3(0.0f)) :
		BLight(c, l, q, a, d, s), direction(dir) {}
	virtual void SendToShader(const Shader& shader);
	void ChangeDirection(const glm::vec3&);
};

class PointLight : public virtual BLight
{
	glm::vec3 position;
	public:
	PointLight(float c = 0.0f, float l = 0.0f, float q = 0.0f, glm::vec3 a = glm::vec3(0.0f), glm::vec3 d = glm::vec3(0.0f), glm::vec3 s = glm::vec3(0.0f), glm::vec3 p = glm::vec3(0.0f)):
		BLight(c,l,q,a,d,s),position(p){}
	Transformation tr;
	/*Set new pos*/
	void SetPos(const glm::vec3& p);
	/*Return current pos*/
	glm::vec3 GetPos() const;
	virtual void SendToShader(const Shader& shader);
};

class SpotLight: public PointLight,public DirectionalLight
{
	float Theta; //angel of big cone
	float Alpha; //angel of small cone
	public:
		SpotLight(float c = 0.0f, float l = 0.0f, float q = 0.0f, glm::vec3 a = glm::vec3(0.0f),
			glm::vec3 d = glm::vec3(0.0f), glm::vec3 s = glm::vec3(0.0f), glm::vec3 p = glm::vec3(0.0f),glm::vec3 dir = glm::vec3(0.0f),
			float BigAngel = 60, float SmallAngel = 30):
			PointLight(c, l, q, a, d, s, p),
			DirectionalLight(c, l, q, a, d, s,dir),
			Theta(BigAngel),Alpha(SmallAngel){}

	virtual void SendToShader(const Shader& shader);
	std::pair<float, float> GetAngels() const;
	void SetAngels(const std::pair<float, float>& NewAngels);
};


enum class LightTypes
{
	Directional = 0, Point, Spot
};


class Light:public SpotLight
{
	LightTypes Type = LightTypes::Point;
	public:
		/*c,l,q - attenuation*/
		Light(float constant = 0.0f, float linear = 0.0f, float quadric = 0.0f, glm::vec3 ambient = glm::vec3(0.0f),
			glm::vec3 diffuse = glm::vec3(0.0f), glm::vec3 specular = glm::vec3(0.0f), glm::vec3 position = glm::vec3(0.0f), glm::vec3 direction = glm::vec3(0.0f),
			float BigAngel = 60, float SmallAngel = 30):
			BLight(constant, linear, quadric, ambient, diffuse, specular),
			SpotLight(constant, linear, quadric, ambient, diffuse, specular, position, direction, BigAngel, SmallAngel) {}
		void SendToShader(const Shader& shader) final;
		void SetType(LightTypes);
		LightTypes GetType() const;
};

