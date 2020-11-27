#include "Light.h"

void Light::SendToShader(std::shared_ptr<Shader> shader)
{
	glUniform3f(glGetUniformLocation(shader->Program, "light.ambient"), ambient.x, ambient.y, ambient.z);
	glUniform3f(glGetUniformLocation(shader->Program, "light.diffuse"), diffuse.x, diffuse.y, diffuse.z);
	glUniform3f(glGetUniformLocation(shader->Program, "light.specular"), specular.x, specular.y, specular.z);
	glUniform1f(glGetUniformLocation(shader->Program, "light.constant"), constant);
	glUniform1f(glGetUniformLocation(shader->Program, "light.linear"),   linear);
	glUniform1f(glGetUniformLocation(shader->Program, "light.quadratic"), quadratic);
}

void PointLight::SendToShader(std::shared_ptr<Shader> shader) 
{
	Light::SendToShader(shader);

	glUniform3f(glGetUniformLocation(shader->Program, "light.position"), position.x, position.y, position.z);
}

glm::vec3 PointLight::SetPos(glm::vec3 p) 
{
	glm::vec3 OldPos = position;
	position = p;
	return OldPos;
}

glm::vec3 PointLight::GetPos() const
{
	return position;
}