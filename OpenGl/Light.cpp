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

void DirectionalLight::SendToShader(const Shader& shader)
{
	BLight::SendToShader(shader);
	shader.setVec("light.direction",direction);
}

glm::vec3 DirectionalLight::ChangeDirection(glm::vec3 NewDir)
{
	std::swap(NewDir, direction);
	return NewDir;
}

void PointLight::SendToShader(const Shader& shader)
{
	BLight::SendToShader(shader);
	shader.setVec("light.position",position);
}
glm::vec3 PointLight::SetPos(glm::vec3 p) 
{
	std::swap(p, position);
	return p;
}

glm::vec3 PointLight::GetPos() const
{
	return position;
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

std::pair<float, float> SpotLight::SetAngels(std::pair<float, float> NewAngels) 
{
	std::pair<float, float> Old = std::make_pair(Theta, Alpha);
	Theta = NewAngels.first;
	Alpha = NewAngels.second;
	return Old;
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