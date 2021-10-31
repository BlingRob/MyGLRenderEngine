#pragma once
#include "Headers.h"

struct Vertexess
{
	Vertexess(std::array<std::size_t, 5> sizes, float* pos, float* nor, float* tc, float* tan, float* bits):
		_msizes(std::move(sizes)), Positions(pos), Normals(nor), TexCoords(tc), Tangents(tc), Bitangents(bits){}
	std::array<std::size_t, 5> _msizes;
	// positions
	float* Positions;
	// normals
	float* Normals;
	// texCoords
	float* TexCoords;
	// tangents
	float* Tangents;
	// bitangents
	float* Bitangents;
	
};

const std::array<std::size_t, 5> CoordsPerPoint({ 3,3,2,3,3 });

class Manager
{
	public:
		Manager() 
		{
			MatrixMemPtr = new unsigned char[AmountMaxtrix * (sizeof(glm::mat4) + sizeof(glm::mat3))];

			Matrix4.push(new (MatrixMemPtr) glm::mat4[AmountMaxtrix]());
			Matrix3.push(new (MatrixMemPtr + AmountMaxtrix * sizeof(glm::mat4) ) glm::mat3[AmountMaxtrix]());
			for (uint16_t i = 0; i < AmountMaxtrix - 1; ++i)
			{
				Matrix4.push(Matrix4.top() + 1);
				Matrix3.push(Matrix3.top() + 1);
			}

			VertMemPtr = new unsigned char[MemoryForVertices];
			FreeBlocks.push_back(std::make_pair(MemoryForVertices, VertMemPtr));
		}

		//template<typename T>
		//	std::shared_ptr<T> GetMat();
		template <typename T>
			std::shared_ptr<T> GetMat()
			{
				std::stack<T*>* st;
				if constexpr (std::is_same < T, glm::mat4 >::value)
					st = &Matrix4;
				else
					st = &Matrix3;

				T* ptr = st->top();
				st->pop();
				*ptr = glm::one<T>();
				return std::shared_ptr<T>(ptr, [=](T* p) {st->push(p); });
			}
		//extern template std::shared_ptr<glm::mat4> GetMat();
		//std::shared_ptr<glm::mat4> GetMat4();
		//std::shared_ptr<glm::mat3> GetMat3();

		std::shared_ptr<Vertexess> GetVertexes(std::array<std::size_t, 5> sizes);

		~Manager() 
		{
			delete[] MatrixMemPtr;//glm::mat is simple class without pointers, can delete all memory
			delete[] VertMemPtr;
		}
	private:
		//Matrixes
		const std::size_t AmountMaxtrix = 5000;
		unsigned char* MatrixMemPtr;

		std::stack<glm::mat4*> Matrix4;
		std::stack<glm::mat3*> Matrix3;
		//Vertexes
		const std::size_t MemoryForVertices = 200 * 1024 * 1024;// 200 Mbi
		unsigned char* VertMemPtr;
		std::list<std::pair<std::size_t, unsigned char*>> FreeBlocks;

};