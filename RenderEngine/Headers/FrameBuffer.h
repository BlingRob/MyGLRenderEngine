#pragma once
#include <memory>
#include <random>
#include "gl/gl.h"
#include "Shader.h"
#include "Transformation.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

enum class BufferType{Color, Depth, Stencil, Depth_Stencil};

class FrameBuffer
{
	private:
		std::tuple<GLenum, GLenum, GLenum> GetGlBufferType(BufferType BuffType);
		bool RenderInclude, TextureInclude;
	protected:
		GLuint _mWeight, _mHeight;
		GLuint textureID, FBO, Render;
		GLint OldFBO;
	public:
		FrameBuffer(const FrameBuffer& fr) = default;
		FrameBuffer(FrameBuffer&& fr) noexcept;
		FrameBuffer(GLuint weight, GLuint height);
		~FrameBuffer();

		virtual void AddFrameBuffer(BufferType BuffType);
		void AddRenderBuffer(BufferType BuffType);

		bool IsCorrect();

		void AttachBuffer();
		void DetachBuffer();

		virtual void SendToShader(const Shader& sh, std::string_view NameUniform, std::uint16_t BindIndex);
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
		std::shared_ptr<Shader> shader;
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
		const std::uint16_t PointBindTexture = 10;
};