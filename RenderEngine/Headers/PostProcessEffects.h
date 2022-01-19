#pragma once
#include <memory>
#include <iostream>
#include "Shader.h"
#include "FrameBuffer.h"
#include "OGLSpec/OGLMesh.h"

class ProcessEffect:public FrameBuffer
{
public:
	ProcessEffect(uint32_t MaxWeight, uint32_t MaxHeight);
	~ProcessEffect();
	void Draw(uint64_t CurWeight, uint64_t CurHeight);

	bool invertion = false;
	bool convolution = false;
	std::shared_ptr<glm::mat3> Core;
private:
	const std::uint16_t PointBindTexture = 10;
	const float VerticesOfQuad[12] =
	{
		-1.0f, 1.0f, 0.0f,
		-1.0f,-1.0f, 0.0f,
		 1.0f,-1.0f, 0.0f,
		 1.0f, 1.0f, 0.0f
	};

	std::vector<uint32_t> Indices =
	{
		0, 1, 2,
		0, 2, 3
	};
	GLuint VAO, VBO, EBO;
	std::unique_ptr<Mesh> _pMesh;
	std::shared_ptr<Shader> _pShader;

};