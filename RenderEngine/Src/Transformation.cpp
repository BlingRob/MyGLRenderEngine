#include "../Headers/Transformation.h"

void Matrices::SendToShader(const Shader& shader)
{
	shader.setMat("transform.view", *View);
	shader.setMat("transform.projection", *Projection);
	shader.setMat("transform.PV", (*Projection) * (*View));
	//shader.setMat("invView", glm::inverse((*View)));
}

void Transformation::Set(const std::shared_ptr <glm::mat4> matr)
{
	Model = std::shared_ptr <glm::mat4>(matr);
	updateNormal();
}
std::shared_ptr <glm::mat4> Transformation::Get() const
{
	return Model;
}

void Transformation::SendToShader(const Shader& shader)
{
	shader.setMat("transform.model", *Model);
	//normal matrix
	shader.setMat("NormalMatrix", NormalMatrix);
}

void Transformation::Translate(const glm::vec3& trans) 
{
	*Model = glm::translate(*Model, trans);
	updateNormal();
}

void Transformation::Rotate(float alph, const glm::vec3& axes)
{
	*Model = glm::rotate(*Model, alph, axes);
	updateNormal();
}
void Transformation::Scale(const glm::vec3& coefs) 
{
	*Model = glm::scale(*Model, coefs);
	updateNormal();
}
void Transformation::updateNormal() 
{
	NormalMatrix = glm::mat3(glm::transpose(glm::inverse(*Model)));
}