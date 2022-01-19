#pragma once
#include "Shader.h"
#include "Transformation.h"
#include "Entity.h"
#include "Shadow.h"
#include "Model.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <type_traits>
#include <functional>

enum class LightTypes
{
	None = 0, Directional, Point, Spot
};

class BLight
{
public:
	BLight(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s,
		const glm::vec3& CLQ, const glm::vec4& LVec):
		_mAmbient(a), _mDiffuse(d), _mSpecular(s), _mCLQ(CLQ), LightVec(LVec)
	{
		if (LightIdxes.empty())
			return;

		id = *LightIdxes.begin();
		LightIdxes.pop_front();

		char buffer[32];
		snprintf(buffer, 32, "light[%d].", id);
		_mStrNumLight = std::string(buffer);

	}
	BLight(const BLight&) = default;
	BLight(BLight&& bl) : _mAmbient(bl._mAmbient), _mDiffuse(bl._mDiffuse), _mSpecular(bl._mSpecular), _mCLQ(bl._mCLQ), LightVec(bl.LightVec),
						  _mStrNumLight(std::move(bl._mStrNumLight))
	{
		std::swap(id,bl.id);
		std::swap(_pShadow,bl._pShadow);
	}
	~BLight();

	glm::vec3 GetAmbient() const;
	glm::vec3 GetDiffuse() const;
	glm::vec3 GetSpecular() const;
	glm::vec3 GetAttenuation() const;
	const Shadow* GetShadow() const;

	void SetAmbient(const glm::vec3&);
	void SetDiffuse(const glm::vec3&);
	void SetSpecular(const glm::vec3&);
	void SetAttenuation(const glm::vec3&);

	virtual void SendToShader(const Shader& shader);
	void DrawShadow(Shadow::Models& models);
	virtual LightTypes GetType();

	private:

	glm::vec3 _mAmbient;
	glm::vec3 _mDiffuse;
	glm::vec3 _mSpecular;
	glm::vec3 _mCLQ;
	/*For point light - position, for direction - dir*/
	glm::vec4 LightVec;

	static inline std::list<int32_t> LightIdxes{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
	int32_t id = -1;

	protected:
	std::shared_ptr<Shadow> _pShadow;

	void SetVec(const glm::vec4&);
	glm::vec4 GetVec() const;

	std::string _mStrNumLight;
};

class DirectionalLight:public BLight
{
public:
	DirectionalLight(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s,
		const glm::vec3& CLQ, const glm::vec3& dir);
	void ChangeDirection(const glm::vec3&);
	glm::vec3 GetDir() const;
	LightTypes GetType() override { return LightTypes::Directional; };
};

class PointLight : public BLight
{
	public:
		PointLight(const PointLight&) = default;
		PointLight(PointLight&&) = default;
		PointLight(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s,
			const glm::vec3& CLQ, const glm::vec3& p);
		/*Set new pos*/
		void SetPos(const glm::vec3& p);
		/*Return current pos*/
		glm::vec3 GetPos() const;
		LightTypes GetType() override { return LightTypes::Point; };
		std::shared_ptr<Model> GetModel();
		void SetModel(std::shared_ptr<Model>);
		void Draw();
	private:
		std::shared_ptr<Model> _pPointLightModel;
};

class SpotLight: public DirectionalLight
{
	float Theta; //angel of big cone
	float Alpha; //angel of small cone
	public:
		SpotLight(const SpotLight&) = default;
		SpotLight(SpotLight&&) = default;
		SpotLight(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s,
			const glm::vec3& CLQ, const glm::vec3& p, const glm::vec3& dir,
			float BigAngel = 60, float SmallAngel = 30): DirectionalLight(a, d, s, CLQ, dir),
			Theta(BigAngel),Alpha(SmallAngel){}

		std::pair<float, float> GetAngels() const;
		void SetAngels(const std::pair<float, float>& NewAngels);
		void SendToShader(const Shader& shader) override;
		LightTypes GetType() override { return LightTypes::Spot; };
};

class Light_Interface
{
public:
	Light_Interface(BLight* _plight = nullptr);
	glm::vec3 GetAmbient() const;
	glm::vec3 GetDiffuse() const;
	glm::vec3 GetSpecular() const;
	glm::vec3 GetAttenuation() const;
	void SetAmbient(const glm::vec3&);
	void SetDiffuse(const glm::vec3&);
	void SetSpecular(const glm::vec3&);
	void SetAttenuation(const glm::vec3&);

	std::pair<float, float> GetAngels() const;
	void SetAngels(const std::pair<float, float>& NewAngels);

	/*Set new pos*/
	void SetPos(const glm::vec3& p);
	/*Return current pos*/
	glm::vec3 GetPos() const;
	std::shared_ptr<Model> GetModel();
	void SetModel(std::shared_ptr<Model>);
	void Draw();

	void ChangeDirection(const glm::vec3&);
	glm::vec3 GetDir() const;
	~Light_Interface();

	BLight* _pLight;
private:
	PointLight* _pPoint;
	DirectionalLight* _pDir;
	SpotLight* _pSpot;
};

class Light:public Entity,public Light_Interface
{
public:
	/*c,l,q - attenuation*/
	Light() {};
	Light(const Light& l) = default;
	Light(Light&& l) = default;

	~Light();
	template<typename L, typename = std::enable_if_t<std::is_base_of<BLight, std::decay_t<L>>::value>>
	explicit Light(L&& lig):Light_Interface(dynamic_cast<BLight*>(new L(std::forward<L>(lig))))// : BLight(std::forward<L>(lig)),std::remove_reference_t<L>(std::forward<L>(lig))
	{
		_mFBO.push_back(Light_Interface::_pLight->GetShadow()->GetFBO());
		Initialized = true;
	}
	template<typename L, typename = std::enable_if_t<std::is_base_of<BLight, std::decay_t<L>>::value>>
	explicit Light(L* lig) :Light_Interface(dynamic_cast<BLight*>(lig))// : BLight(std::forward<L>(lig)),std::remove_reference_t<L>(std::forward<L>(lig))
	{
		_mFBO.push_back(Light_Interface::_pLight->GetShadow()->GetFBO());
		Initialized = true;
	}

	void SendToShader(const Shader& shader);
	void DrawShadows(Shadow::Models models);
	static void ClearBuffers();
	LightTypes GetType() const;
	bool IsInit() { return Initialized; };
	private:
	bool Initialized{ false };

	static inline std::list <GLuint> _mFBO;
}; 
