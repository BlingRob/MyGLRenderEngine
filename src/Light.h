#pragma once
#include "Headers.h"
#include "Shader.h"
#include "Transformation.h"
#include "Entity.h"
#include "Shadow.h"

class BLight:public Entity,public Shadow
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	protected:
	std::string lightStr;
	public:
	BLight(glm::vec3 a = glm::vec3(0.0f), glm::vec3 d = glm::vec3(0.0f), glm::vec3 s = glm::vec3(0.0f))
		{
			ambient = a;
			diffuse = d;
			specular = s;
			char buffer[32];
			snprintf(buffer, 32, "light[%d].", Shadow::id);
			lightStr = std::string(buffer);
		}
	glm::vec3 GetAmbient() const;
	glm::vec3 GetDiffuse() const;
	glm::vec3 GetSpecular() const;

	void SetAmbient(const glm::vec3&);
	void SetDiffuse(const glm::vec3&);
	void SetSpecular(const glm::vec3&);
	
	virtual void SendToShader(const Shader& shader);
};

class DirectionalLight:public virtual BLight
{
	glm::vec3 direction;

	//GLuint ShadowMapSize = 1024;
	//std::unique_ptr<ShadowMapBuffer> ShadowMap;
	//Matrices LightMat;//Translation-projection Shadow matrices
	//glm::mat4 scale_bias_matrix;
	//using const_model_iterator = std::map<std::size_t, std::shared_ptr<Model>>::const_iterator;
public:
	DirectionalLight(glm::vec3 a = glm::vec3(0.0f), glm::vec3 d = glm::vec3(0.0f), glm::vec3 s = glm::vec3(0.0f), glm::vec3 dir = glm::vec3(0.0f));
	//void ShadowDraw(std::pair<const_model_iterator, const_model_iterator> models,uint16_t NumLights);
	virtual void SendToShader(const Shader& shader);
	void ChangeDirection(const glm::vec3&);
};

class PointLight : public virtual BLight
{
	glm::vec3 position;
	float constant;
	float linear;
	float quadratic;

	//Matrices LightMat;//Translation-projection Shadow matrices

	//using const_model_iterator = std::map<std::size_t, std::shared_ptr<Model>>::const_iterator;
	protected:
		static const uint16_t MAX_POINT_LIGHTS = 10;
	public:
		PointLight(glm::vec3 a = glm::vec3(0.0f), glm::vec3 d = glm::vec3(0.0f), glm::vec3 s = glm::vec3(0.0f), glm::vec3 p = glm::vec3(0.0f), float c = 0.0f, float l = 0.0f, float q = 0.0f);

	void SetAttenuation(const glm::vec3&);
	glm::vec3 GetAttenuation() const;
	//void ShadowDraw(std::pair<const_model_iterator, const_model_iterator> models, uint16_t NumLights);

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
		SpotLight(glm::vec3 a = glm::vec3(0.0f),glm::vec3 d = glm::vec3(0.0f), glm::vec3 s = glm::vec3(0.0f),
			glm::vec3 p = glm::vec3(0.0f),glm::vec3 dir = glm::vec3(0.0f),float c = 0.0f, float l = 0.0f, float q = 0.0f,
			float BigAngel = 60, float SmallAngel = 30):
			PointLight(a, d, s, p, c, l, q),
			DirectionalLight(a, d, s, dir),
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
		Light(glm::vec3 ambient = glm::vec3(0.0f),glm::vec3 diffuse = glm::vec3(0.0f), glm::vec3 specular = glm::vec3(0.0f),
			float constant = 0.0f, float linear = 0.0f, float quadric = 0.0f, glm::vec3 position = glm::vec3(0.0f), glm::vec3 direction = glm::vec3(0.0f),
			float BigAngel = 60, float SmallAngel = 30):
			BLight(ambient, diffuse, specular),
			SpotLight(ambient, diffuse, specular, position, direction, constant, linear, quadric, BigAngel, SmallAngel) {}
		//Light(SpotLight);
		void SendToShader(const Shader& shader) final;
		void DrawShadows(std::pair<Shadow::const_model_iterator, const_model_iterator> models);
		void SetType(LightTypes);
		LightTypes GetType() const;
		static bool Init();
		void SendShadowsToShader();
};

