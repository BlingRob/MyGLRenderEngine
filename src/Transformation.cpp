#include "Transformation.h"

void Matrices::SendToShader(const Shader& shader)
{
	shader.setMat("transform.view", *View);
	shader.setMat("transform.projection", *Projection);
	shader.setMat("transform.VP", (*Projection) * (*View));
}

void Transformation::Set(std::shared_ptr <glm::mat4> matr)
{
	Model = matr;
}
const std::shared_ptr <glm::mat4> Transformation::Get() const 
{
	return Model;
}

void Transformation::SendToShader(const Shader& shader)
{
	shader.setMat("transform.model", *Model);
	//normal matrix
	shader.setMat("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(*Model))));
}

void Transformation::Translate(const glm::vec3& trans) 
{
	*Model = glm::translate(*Model, trans);
}

void Transformation::Rotate(float alph, const glm::vec3& axes)
{
	*Model = glm::rotate(*Model, alph, axes);
}
void Transformation::Scale(const glm::vec3& coefs) 
{
	*Model = glm::scale(*Model, coefs);
}