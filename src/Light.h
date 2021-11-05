#pragma once
#include "Headers.h"
#include "Shader.h"
#include "Transformation.h"
#include "Entity.h"
#include "Shadow.h"
#include <type_traits>

class BLight
{
public:
	BLight() {}
	BLight(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s,
		const glm::vec3& CLQ):
		_mAmbient(a), _mDiffuse(d), _mSpecular(s), _mCLQ(CLQ) {
	}
	BLight(const BLight& bl) : _mAmbient(bl._mAmbient), _mDiffuse(bl._mDiffuse), _mSpecular(bl._mSpecular), _mCLQ(bl._mCLQ)
	{ }
	glm::vec3 GetAmbient() const;
	glm::vec3 GetDiffuse() const;
	glm::vec3 GetSpecular() const;
	glm::vec3 GetAttenuation() const;

	void SetAmbient(const glm::vec3&);
	void SetDiffuse(const glm::vec3&);
	void SetSpecular(const glm::vec3&);
	void SetAttenuation(const glm::vec3&);

	void SendToShader(const Shader& shader);

	private:

	glm::vec3 _mAmbient;
	glm::vec3 _mDiffuse;
	glm::vec3 _mSpecular;
	glm::vec3 _mCLQ;

	protected:
	std::string _mStrNumLight;
	void SetNumStr(const std::string&);
};

class DirectionalLight:public virtual BLight
{
protected:
	glm::vec3 direction;
	std::shared_ptr<DirectionShadow> _mShadow;
public:
	DirectionalLight() {}
	DirectionalLight(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s,
		const glm::vec3& CLQ, const glm::vec3& dir);
	DirectionalLight(const DirectionalLight& dl):BLight(dl)
	{
		direction = dl.direction;
		_mShadow = dl._mShadow;
	}
	virtual void SendToShader(const Shader& shader);
	void ChangeDirection(const glm::vec3&);
	glm::vec3 GetDir();
};

class PointLight : public virtual BLight
{
	protected:
	glm::vec3 position;
	std::shared_ptr<PointShadow> _mShadow;
	public:
		PointLight() {};
		PointLight(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s,
			const glm::vec3& CLQ, const glm::vec3& p);
		PointLight(const PointLight& pl):BLight(pl)
		{
			tr = pl.tr;
			_mShadow = pl._mShadow;
		}

	Transformation tr;
	/*Set new pos*/
	void SetPos(const glm::vec3& p);
	/*Return current pos*/
	glm::vec3 GetPos() const;
	virtual void SendToShader(const Shader& shader);
};

class SpotLight : public PointLight, public DirectionalLight
{
	float Theta; //angel of big cone
	float Alpha; //angel of small cone
	public:
		explicit SpotLight(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s,
			const glm::vec3& CLQ, const glm::vec3& p, const glm::vec3& dir,
			float BigAngel = 60, float SmallAngel = 30)://BLight(a,d,s, CLQ),
			PointLight(a, d, s, CLQ, p), DirectionalLight(a, d, s, CLQ, dir),
			Theta(BigAngel),Alpha(SmallAngel){}
		template<typename L>
		explicit SpotLight(const L& sp): BLight(sp), L(sp)
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

class Light_Indexes
{
	public:
	GLuint _Light_id;
	GLint _Shadow_id;
	GLuint _FBO_id;
	template<typename Light_t>
	Light_Indexes(const Light_t&)
	{
		if (Shadow_types<Light_t>::ShadowIdxes.empty())
			throw("Limit of shadows is exceeded!");
		_Shadow_id = *Shadow_types<Light_t>::ShadowIdxes.begin();
		Shadow_types<Light_t>::ShadowIdxes.pop_front();

		if (LightIdxes.empty())
			throw("Light limit is exceeded!");
		_Light_id = *LightIdxes.begin();
		LightIdxes.pop_front();

		Destructor = [&,this]()
		{
			Shadow_types<Light_t>::ShadowIdxes.insert(std::lower_bound(Shadow_types<Light_t>::ShadowIdxes.begin(), Shadow_types<Light_t>::ShadowIdxes.end(), _Shadow_id), _Shadow_id);
		};
	}

	~Light_Indexes() 
	{
		LightIdxes.insert(std::lower_bound(LightIdxes.begin(), LightIdxes.end(), _Light_id), _Light_id);
		Destructor();
	}
	private:
	
	template<typename Light_t>
	struct Shadow_types
	{
		static inline std::list<GLint> ShadowIdxes{ 0, 1, 2, 3, 4};
	};

	std::function<void()> Destructor;

	static inline std::list<GLuint> LightIdxes{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
};

class Light:public Entity, public SpotLight
{
public:
	using const_model_iterator = std::map<std::size_t, std::shared_ptr<Model>>::const_iterator;
	/*c,l,q - attenuation*/
	Light() = delete;
	~Light();
	template<typename L>
	explicit Light(L lig) : BLight(lig), SpotLight(lig)
	{
		try 
		{
			Indexes = std::make_shared<Light_Indexes>(lig);
		}
		catch (const char* msg) 
		{
			throw("Impossible create light");
		}

		if constexpr (std::is_same<L, PointLight>::value)
		{
			Type = LightTypes::Point;
			Indexes->_FBO_id = L::_mShadow->AddBuffer(Indexes->_Shadow_id);
		}
		else if constexpr (std::is_same<L, DirectionalLight>::value)
		{
			Type = LightTypes::Directional;
			Indexes->_FBO_id = L::_mShadow->AddBuffer(Indexes->_Shadow_id);
		}
		else
		{
			Type = LightTypes::Spot;
		}

		char buffer[32];
		if (Indexes)
		{
			//snprintf(buffer, 32, "LightPositions[%d]", Indexes->_Light_id);
			//StrLightPos = std::string(buffer);
			snprintf(buffer, 32, "light[%d].", Indexes->_Light_id);
			StrNumLight = std::string(buffer);
			BLight::SetNumStr(StrNumLight);
			_mFBO.push_back(Indexes->_FBO_id);
			StrNumLight = StrNumLight + "index";
		}

	}

	void SendToShader(const Shader& shader);
	void DrawShadows(std::pair<const_model_iterator, const_model_iterator> models);
	static void ClearBuffers();
	LightTypes GetType() const;

	private:
	LightTypes Type = LightTypes::Point;
	//std::string StrLightPos;
	std::string StrNumLight;
	std::shared_ptr<Light_Indexes> Indexes;

	static inline std::list <GLuint> _mFBO;
}; 
