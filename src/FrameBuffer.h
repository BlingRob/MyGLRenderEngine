#pragma once
#include "Headers.h"
#include "Shader.h"

enum class BufferType{Color, Depth, Stencil, Depth_Stencil};

class FrameBuffer
{
	GLuint _mWeight, _mHeight;
	GLuint FBO;
	GLuint textureID,Render;
	std::tuple<GLenum, GLenum, GLenum> GetGlBufferType(BufferType BuffType);

	std::unique_ptr<Shader> shader;

	GLfloat VerticesOfQuad[8] =
	{
		-1.0f, 1.0f,
		-1.0f,-1.0f,
		 1.0f,-1.0f,
		 1.0f, 1.0f,
	};
	GLuint Indices[6] =
	{
		0, 1, 2,
		0, 2, 3
	};

	GLuint VAO, VBO, EBO;

	public:
		FrameBuffer(GLuint weight, GLuint height);
		~FrameBuffer();

		void AddFrameBuffer(BufferType BuffType);
		void AddRenderBuffer(BufferType BuffType);

		bool IsCorrect();

		void AttachBuffer();
		void DetachBuffer();

		void Draw(uint64_t weight, uint64_t height);
		//states
		bool invertion = false;
		bool convolution = false;
		std::shared_ptr<glm::mat3> Core;
};