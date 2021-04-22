#pragma once
#include "Headers.h"
#include "Shader.h"

enum class BufferType{Color, Depth, Stencil, Depth_Stencil};

class FrameBuffer
{
	GLuint buff;
	GLuint textureID,Render;
	std::tuple<GLenum, GLenum, GLenum> GetGlBufferType(BufferType BuffType);

	std::unique_ptr<Shader> shader;

	GLfloat VerticesOfQuad[12] =
	{
		0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,
		1.0f,0.0f,0.0f,
		1.0f,1.0f,0.0f
	};
	GLuint Indices[6] =
	{
		0, 1, 2,
		0, 2, 3
	};

	GLuint VAO, VBO, EBO;

	public:
		FrameBuffer();
		~FrameBuffer();

		void AddFrameBuffer(BufferType BuffType, GLuint weight, GLuint height);
		void AddRanderBuffer(BufferType BuffType, GLuint weight, GLuint height);

		bool IsCorrect();

		void AttachBuffer();
		void DetachBuffer();

		void Draw();
};