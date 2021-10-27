#include "MemoryManager.h"

std::shared_ptr<glm::mat4> Manager::GetMat4() 
{
	glm::mat4* ptr = Matrix4.top();
	Matrix4.pop();
	*ptr = glm::one<glm::mat4>();
	return std::shared_ptr<glm::mat4>(ptr, [&](glm::mat4* p) {Matrix4.push(p); });
}

std::shared_ptr<glm::mat3> Manager::GetMat3()
{
	glm::mat3* ptr = Matrix3.top();
	Matrix3.pop();
	return std::shared_ptr<glm::mat3>(ptr, [&](glm::mat3* p) {Matrix3.push(p); });
}