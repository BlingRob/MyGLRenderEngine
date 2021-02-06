#include "Light.h"

void BLight::SendToShader(std::shared_ptr<Shader> shader)
{
	glUniform3f(glGetUniformLocation(shader->Program, "light.ambient"), ambient.x, ambient.y, ambient.z);
	glUniform3f(glGetUniformLocation(shader->Program, "light.diffuse"), diffuse.x, diffuse.y, diffuse.z);
	glUniform3f(glGetUniformLocation(shader->Program, "light.specular"), specular.x, specular.y, specular.z);
	glUniform1f(glGetUniformLocation(shader->Program, "light.constant"), constant);
	glUniform1f(glGetUniformLocation(shader->Program, "light.linear"),   linear);
	glUniform1f(glGetUniformLocation(shader->Program, "light.quadratic"), quadratic);
}


void DirectionalLight::SendToShader(std::shared_ptr<Shader> shader)
{
	BLight::SendToShader(shader);
	glUniform3f(glGetUniformLocation(shader->Program, "light.direction"), direction.x, direction.y, direction.z);
}

glm::vec3 DirectionalLight::ChangeDirection(glm::vec3 NewDir)
{
	std::swap(NewDir, direction);
	return NewDir;
}

void PointLight::SendToShader(std::shared_ptr<Shader> shader)
{
	BLight::SendToShader(shader);

	glUniform3f(glGetUniformLocation(shader->Program, "light.position"), position.x, position.y, position.z);
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

void SpotLight::SendToShader(std::shared_ptr<Shader> shader) 
{
	PointLight::SendToShader(shader);
	glUniform1f(glGetUniformLocation(shader->Program, "light.Theta"),Theta);
	glUniform1f(glGetUniformLocation(shader->Program, "light.Alpha"), Alpha);
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
void Light::SendToShader(std::shared_ptr<Shader> shader)
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