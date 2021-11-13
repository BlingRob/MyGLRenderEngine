#include "Light.h"

void BLight::SendToShader(const Shader& shader)
{
	shader.setVec((_mStrNumLight + "ambient"), _mAmbient);
	shader.setVec((_mStrNumLight + "diffuse"), _mDiffuse);
	shader.setVec((_mStrNumLight + "specular"), _mSpecular);
	shader.setVec((_mStrNumLight + "clq"), _mCLQ);
}

glm::vec3 BLight::GetAmbient() const 
{
	return _mAmbient;
}
glm::vec3 BLight::GetDiffuse() const
{
	return _mDiffuse;
}
glm::vec3 BLight::GetSpecular() const
{
	return _mSpecular;
}
glm::vec3 BLight::GetAttenuation() const
{
	return _mCLQ;
}
void BLight::SetAmbient(const glm::vec3& amb)
{
	_mAmbient = amb;
}
void BLight::SetSpecular(const glm::vec3& spec)
{
	_mSpecular = spec;
}
void BLight::SetDiffuse(const glm::vec3& dif)
{
	_mDiffuse = dif;
}
void BLight::SetAttenuation(const glm::vec3& CLQ)
{
	_mCLQ = CLQ;
}
void BLight::SetNumStr(const std::string& str)
{
	_mStrNumLight = str;
}

DirectionalLight::DirectionalLight(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s, const glm::vec3& CLQ, const glm::vec3& dir) :
BLight(a, d, s, CLQ), direction(dir)
{
	_mShadow = std::make_shared<DirectionShadow>();
}

void DirectionalLight::SendToShader(const Shader& shader)
{
	shader.setVec((_mStrNumLight + "LightPositions"), glm::vec4(direction, 0.0f));
	_mShadow->SendToShader(shader);
}
void PointLight::SendToShader(const Shader& shader)
{
	shader.setVec((_mStrNumLight + "LightPositions"), glm::vec4(GetPos(), 1.0f));
	_mShadow->SendToShader(shader);
}

void DirectionalLight::ChangeDirection(const glm::vec3& NewDir)
{
	direction = NewDir;
}
glm::vec3 DirectionalLight::GetDir() 
{
	return direction;
}

PointLight::PointLight(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s, const glm::vec3& CLQ, const glm::vec3& p):
BLight(a, d, s, CLQ)
{
	SetPos(p);
	_mShadow = std::make_shared<PointShadow>();
}

void PointLight::SetPos(const glm::vec3& p)
{
	(*tr.Get())[3] = glm::vec4(p,1.0f);
}

glm::vec3 PointLight::GetPos() const
{
	return (*tr.Get())[3];
}

void SpotLight::SendToShader(const Shader& shader)
{
	PointLight::SendToShader(shader);
	shader.setScal("light.Theta", Theta);
	shader.setScal("light.Alpha", Alpha);
}

std::pair<float, float> SpotLight::GetAngels() const
{
	return std::make_pair(Theta,Alpha);
}

void SpotLight::SetAngels(const std::pair<float, float>& NewAngels) 
{
	Theta = NewAngels.first;
	Alpha = NewAngels.second;
}

LightTypes Light::GetType() const
{
	return Type;
}

void Light::SendToShader(const Shader& shader)
{
	BLight::SendToShader(shader);
	shader.setScal(StrNumLight, Indexes->_Shadow_id);
	switch (Type)
	{
		case LightTypes::Directional:
			DirectionalLight::SendToShader(shader);
		break;
		case LightTypes::Spot:
			SpotLight::SendToShader(shader);
		break;
		case LightTypes::Point:
		default:
			PointLight::SendToShader(shader);
		break;
	}
}

void Light::DrawShadows(std::pair<const_model_iterator, const_model_iterator> models)
{
	switch (Type)
	{
	case LightTypes::Directional:
		DirectionalLight::_mShadow->Draw(models, glm::vec4(DirectionalLight::GetDir(), 0.0f));
		break;
	case LightTypes::Spot:
		//SpotLight::Draw(models, glm::vec4(PointLight::GetPos(), 1.0f));
		break;
	case LightTypes::Point:
	default:
		PointLight::_mShadow->Draw(models, glm::vec4(PointLight::GetPos(), 1.0f));
		break;
	}
}

void Light::ClearBuffers()
{
	const GLfloat depthVal = 1.0f;
	if (!_mFBO.empty()) 
		for(const auto& fbo:_mFBO)
			glClearNamedFramebufferfv(fbo, GL_DEPTH, 0, &depthVal);
}

Light::~Light()
{
	_mFBO.remove(Indexes->_FBO_id);
	switch (Type)
	{
	case LightTypes::Spot:
	case LightTypes::Directional:
		break;
	case LightTypes::Point:
		break;
	}
}