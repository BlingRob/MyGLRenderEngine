#pragma once
#include "Headers.h"
#include "Shader.h"
#include "Transformation.h"
#include "Entity.h"
#include "Shadow.h"
#include <type_traits>

class BLight
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

public:
	BLight() {};
	BLight(const glm::vec3& a, const  glm::vec3& d, const glm::vec3& s):
		ambient(a),diffuse(d),specular(s){
	}
	BLight(const BLight& bl): ambient(bl.ambient), diffuse(bl.diffuse), specular(bl.specular) { };
	glm::vec3 GetAmbient() const;
	glm::vec3 GetDiffuse() const;
	glm::vec3 GetSpecular() const;

	void SetAmbient(const glm::vec3&);
	void SetDiffuse(const glm::vec3&);
	void SetSpecular(const glm::vec3&);
	
	void SendToShader(const std::string& name, const Shader& shader);
};

class DirectionalLight:public virtual BLight, public DirectionShadow
{
protected:
	glm::vec3 direction;

public:
	DirectionalLight() {};
	DirectionalLight(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s, const glm::vec3& dir);
	DirectionalLight(const DirectionalLight& dl) :BLight(dynamic_cast<const BLight&>(dl)), Shadow(dynamic_cast<const Shadow&>(dl)), DirectionShadow(dynamic_cast<const DirectionShadow&>(dl))
	{
		direction = dl.direction;
	}
	virtual void SendToShader(const Shader& shader);
	void ChangeDirection(const glm::vec3&);
	glm::vec3 GetDir();
};

class PointLight : public virtual BLight,public PointShadow
{
	glm::vec3 position;
	glm::vec3 clq;

	public:
		PointLight() {};
		PointLight(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s, const glm::vec3& p, const glm::vec3& clq);
		PointLight(const PointLight& pl):BLight(dynamic_cast<const BLight&>(pl)), Shadow(dynamic_cast<const Shadow&>(pl)), PointShadow(dynamic_cast<const PointShadow&>(pl))
		{
			this->clq = pl.clq;
			tr = pl.tr;
		}
		void SetAttenuation(const glm::vec3&);
		glm::vec3 GetAttenuation() const;

	Transformation tr;
	/*Set new pos*/
	void SetPos(const glm::vec3& p);
	/*Return current pos*/
	glm::vec3 GetPos() const;
	void SendToShader(const Shader& shader);
};

class SpotLight: public virtual PointLight, public virtual DirectionalLight
{
	float Theta; //angel of big cone
	float Alpha; //angel of small cone
	public:
		SpotLight() {};
		SpotLight(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s,
			const glm::vec3& p, const glm::vec3& dir,glm::vec3& CLQ,
			float BigAngel = 60, float SmallAngel = 30):
			PointLight(a, d, s, p, CLQ), DirectionalLight(a, d, s, dir) /*DirectionalLight(a, d, s, dir)*/,
			Theta(BigAngel),Alpha(SmallAngel){}
		template<typename L>
		SpotLight(const L& sp) :PointLight(dynamic_cast<const PointLight&>(sp)), DirectionalLight(dynamic_cast<const DirectionalLight&>(sp)) 
		{
		};

	void SendToShader(const Shader& shader);
	std::pair<float, float> GetAngels() const;
	void SetAngels(const std::pair<float, float>& NewAngels);
};


enum class LightTypes
{
	Directional = 0, Point, Spot
};

class Light:public Entity, public SpotLight
{
	LightTypes Type = LightTypes::Point;
	std::string StrLightPos;
	std::string StrNumLight;

public:
	/*c,l,q - attenuation*/
	Light() = delete;
	~Light();
	template<typename L>
	explicit Light(L lig) : BLight(lig), Shadow(lig), L(lig)
	{
		if constexpr (std::is_same<L, PointLight>::value)
		{
			Type = LightTypes::Point;
			IndexPointFBO.push_back(*FrameBuffer::FBO);
		}
		else if constexpr (std::is_same<L, DirectionalLight>::value)
		{
			Type = LightTypes::Directional;
			IndexDirectionOrSpotFBO.push_back(*FrameBuffer::FBO);
		}
		else
		{
			Type = LightTypes::Spot;
			IndexDirectionOrSpotFBO.push_back(*FrameBuffer::FBO);
		}

		char buffer[32];
		if (!ListOfLights.empty())
		{
			Index = *ListOfLights.begin();
			snprintf(buffer, 32, "LightPositions[%d]", Index);
			StrLightPos = std::string(buffer);
			snprintf(buffer, 32, "light[%d].", Index);
			StrNumLight = std::string(buffer);

			ListOfLights.pop_front();
		}
	}

	void SendToShader(const Shader& shader);
	void DrawShadows(std::pair<const_model_iterator, const_model_iterator> models);
	LightTypes GetType() const;

	GLuint Index;
	static void ClearBuffers();
	static inline std::list <GLuint> ListOfLights{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}, IndexDirectionOrSpotFBO, IndexPointFBO;
}; 
