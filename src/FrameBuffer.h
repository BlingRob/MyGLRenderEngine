#pragma once
#include "Headers.h"
#include "Shader.h"
#include "Transformation.h"

enum class BufferType{Color, Depth, Stencil, Depth_Stencil};

class FrameBuffer
{
	private:
		
		std::tuple<GLenum, GLenum, GLenum> GetGlBufferType(BufferType BuffType);
	protected:
		GLuint _mWeight, _mHeight;
		GLuint textureID;
		GLuint FBO, Render;
		std::shared_ptr<Shader> shader;
	public:
		FrameBuffer(GLuint weight, GLuint height);
		~FrameBuffer();

		virtual void AddFrameBuffer(BufferType BuffType);
		void AddRenderBuffer(BufferType BuffType);

		bool IsCorrect();

		void AttachBuffer();
		void DetachBuffer();

		void SendToShader(std::shared_ptr<Shader> sh, std::string_view NameUniform);
		std::shared_ptr<Shader> GetShader();
};

class ShadowMapBuffer:public FrameBuffer
{
	public:
		ShadowMapBuffer(GLuint weight, GLuint height);
		void AddFrameBuffer(BufferType BuffType);

};
class PostProcessBuffer:public FrameBuffer 
{
	public:
		PostProcessBuffer(GLuint weight, GLuint height);
		~PostProcessBuffer();
		void Draw(uint64_t weight, uint64_t height);
		//states
		bool invertion = false;
		bool convolution = false;
		std::shared_ptr<glm::mat3> Core;
	private:
		GLuint VAO, VBO, EBO;

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
};