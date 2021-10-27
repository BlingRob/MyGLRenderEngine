#include "Light.h"

void BLight::SendToShader(const std::string& name, const Shader& shader)
{
	shader.setVec((name + "ambient"),ambient);
	shader.setVec((name + "diffuse"),diffuse);
	shader.setVec((name + "specular"), specular);
}

glm::vec3 BLight::GetAmbient() const 
{
	return ambient;
}
glm::vec3 BLight::GetDiffuse() const
{
	return diffuse;
}
glm::vec3 BLight::GetSpecular() const
{
	return specular;
}
void BLight::SetAmbient(const glm::vec3& amb)
{
	ambient = amb;
}
void BLight::SetSpecular(const glm::vec3& spec)
{
	specular = spec;
}
void BLight::SetDiffuse(const glm::vec3& dif)
{
	diffuse = dif;
}

DirectionalLight::DirectionalLight(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s, const glm::vec3& dir) :
BLight(a, d, s), direction(glm::vec3(dir)), Shadow(true), DirectionShadow(true)
{
	DirectionShadow::AddBuffer();
}

void DirectionalLight::SendToShader(const Shader& shader)
{
	DirectionShadow::SendToShader(shader);
}
void PointLight::SendToShader(const Shader& shader)
{
	PointShadow::SendToShader(shader);
}

void DirectionalLight::ChangeDirection(const glm::vec3& NewDir)
{
	direction = NewDir;
}
glm::vec3 DirectionalLight::GetDir() 
{
	return direction;
}

void PointLight::SetAttenuation(const glm::vec3& CLQ)
{
	this->clq = CLQ;
}
glm::vec3 PointLight::GetAttenuation() const
{
	return clq;
}

PointLight::PointLight(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s, const glm::vec3& p, const glm::vec3& CLQ):
BLight(a, d, s), Shadow(true), PointShadow(true)
{
	this->clq = CLQ;
	SetPos(p);

	PointShadow::AddBuffer();
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
	BLight::SendToShader(StrNumLight, shader);
	shader.setScal((StrNumLight + "index"), *Shadow::id);
	switch (Type)
	{
		case LightTypes::Directional:
			shader.setMat((StrNumLight + "ShadowMatrix"), scale_bias_matrix * (*LightMat.Projection) * (*LightMat.View));
			shader.setVec((StrNumLight + "clq"), glm::vec3(1.0f, 0.0f, 0.0f));
			shader.setVec((StrNumLight + "LightPositions"), glm::vec4(direction, 0.0f));
			DirectionalLight::SendToShader(shader);
		break;
		case LightTypes::Spot:
			SpotLight::SendToShader(shader);
		break;
		case LightTypes::Point:
		default:
			shader.setVec((StrNumLight + "LightPositions"), glm::vec4(GetPos(), 1.0f));
			shader.setVec((StrNumLight + "clq"), PointLight::GetAttenuation());
			PointLight::SendToShader(shader);
		break;
	}
}

void Light::DrawShadows(std::pair<const_model_iterator, const_model_iterator> models)
{
	switch (Type)
	{
	case LightTypes::Directional:
		DirectionalLight::Draw(models, glm::vec4(DirectionalLight::GetDir(), 0.0f));
		break;
	case LightTypes::Spot:
		//SpotLight::Draw(models, glm::vec4(PointLight::GetPos(), 1.0f));
		break;
	case LightTypes::Point:
	default:
		PointLight::Draw(models, glm::vec4(PointLight::GetPos(), 1.0f));
		break;
	}
}

void Light::ClearBuffers()
{
	const GLfloat depthVal = 1.0f;
	if(!IndexDirectionOrSpotFBO.empty())
		glClearNamedFramebufferfv(IndexDirectionOrSpotFBO.back(), GL_DEPTH, 0, &depthVal);
	if (!IndexPointFBO.empty())
		glClearNamedFramebufferfv(IndexPointFBO.back(), GL_DEPTH, 0, &depthVal);
}

Light::~Light()
{
	switch (Type)
	{
	case LightTypes::Spot:
	case LightTypes::Directional:
		if(!IndexDirectionOrSpotFBO.empty())
			IndexDirectionOrSpotFBO.pop_back();
		break;
	case LightTypes::Point:
		if (!IndexPointFBO.empty())
			IndexPointFBO.pop_back();
		break;
	}
	ListOfLights.insert(std::lower_bound(ListOfLights.begin(), ListOfLights.end(), Index), Index);
}