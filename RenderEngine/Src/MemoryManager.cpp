#include "../Headers/MemoryManager.h"

/*
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
}*/
/*
template <>
std::shared_ptr<glm::mat3> Manager::GetMat()
{
	glm::mat3* ptr = Matrix3.top();
	Matrix3.pop();
	*ptr = glm::one<glm::mat3>();
	return std::shared_ptr<glm::mat3>(ptr, [&](glm::mat3* p) {Matrix3.push(p); });
}

template <>
std::shared_ptr<glm::mat4> Manager::GetMat()
{
	glm::mat4* ptr = Matrix4.top();
	Matrix4.pop();
	*ptr = glm::one<glm::mat4>();
	return std::shared_ptr<glm::mat4>(ptr, [&](glm::mat4* p) {Matrix4.push(p); });
}*/


std::shared_ptr<Vertexess> Manager::GetVertexes(std::array<std::size_t, 5> sizes)
{
	std::array<std::size_t, 5> Biases;
	std::size_t NeedBytes = std::inner_product
	(
		sizes.cbegin(), sizes.cend(), CoordsPerPoint.cbegin(), 0,
		[&Biases](const size_t& x, const size_t& y)
		{
			static size_t i = 0;
			Biases[i++] = x * sizeof(float);
			return x + y;
		},
		std::multiplies<>()
	) * sizeof(float);
	//std::size_t NeedBytes = std::inner_product(sizes.cbegin(),sizes.cend(), CoordsPerPoint.cbegin(), 0) * sizeof(float);
	auto Block = std::lower_bound(FreeBlocks.begin(), FreeBlocks.end(), NeedBytes, [](const std::pair<std::size_t, unsigned char*>& p1, auto val) {return p1.first < val; });
	if (Block != FreeBlocks.end())
	{
		FreeBlocks.push_front(std::make_pair(Block->first - NeedBytes, Block->second + NeedBytes));
		
		return std::make_shared<Vertexess>
			(
				sizes,
				(float*)(Block->second + Biases[0]),
				(float*)(Block->second + Biases[1]),
				(float*)(Block->second + Biases[2]),
				(float*)(Block->second + Biases[3]),
				(float*)(Block->second + Biases[4])
				);
	}
}