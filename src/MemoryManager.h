#pragma once
#include "Headers.h"

class Manager
{
	public:
		Manager() 
		{
			MemPtr = new unsigned char[AmountMaxtrix * (sizeof(glm::mat4) + sizeof(glm::mat3))];
			
			Matrix4.push(new (MemPtr) glm::mat4[AmountMaxtrix]());
			Matrix3.push(new (MemPtr + AmountMaxtrix * sizeof(glm::mat4) ) glm::mat3[AmountMaxtrix]());
			for (uint16_t i = 0; i < AmountMaxtrix - 1; ++i)
			{
				Matrix4.push(Matrix4.top() + 1);
				Matrix3.push(Matrix3.top() + 1);
			}
		}
		std::shared_ptr<glm::mat4> GetMat4();
		std::shared_ptr<glm::mat3> GetMat3();

		~Manager() 
		{
			delete[] MemPtr;//glm::mat is simple class without pointers, can delete all memory
		}
	private:
		//Matrixes
		const std::size_t AmountMaxtrix = 5000;
		unsigned char* MemPtr;
		std::stack<glm::mat4*> Matrix4;
		std::stack<glm::mat3*> Matrix3;
		//Vertexes

};