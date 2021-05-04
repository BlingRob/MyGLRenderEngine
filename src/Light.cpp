#include "Light.h"

void BLight::SendToShader(const Shader& shader)
{
	shader.setVec("light.ambient",ambient);
	shader.setVec("light.diffuse", diffuse);
	shader.setVec("light.specular", specular);
	shader.setScal("light.constant", constant);
	shader.setScal("light.linear", linear);
	shader.setScal("light.quadratic", quadratic);
}
glm::vec3 BLight::GetAttenuation() const 
{
	return glm::vec3(constant, linear, quadratic);
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
void BLight::SetAttenuation(const glm::vec3& CLQ)
{
	constant = CLQ.x;
	linear = CLQ.y;
	quadratic = CLQ.z;
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

void DirectionalLight::SendToShader(const Shader& shader)
{
	BLight::SendToShader(shader);
	shader.setVec("light.direction",direction);
}

void DirectionalLight::ChangeDirection(const glm::vec3& NewDir)
{
	direction = NewDir;
}

void PointLight::SendToShader(const Shader& shader)
{
	BLight::SendToShader(shader);
	shader.setVec("light.position", glm::vec3((*tr.Get())[3]));
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

void Light::SetType(LightTypes type) 
{
	Type = type;
}
LightTypes Light::GetType() const
{
	return Type;
}
void Light::SendToShader(const Shader& shader)
{
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