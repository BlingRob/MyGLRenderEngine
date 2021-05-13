#pragma once
#include "Headers.h"
#include "FrameBuffer.h"
#include "Model.h"
#include "Transformation.h"
#include <array>

class Shadow:public FrameBuffer
{
	protected:
		using const_model_iterator = std::map<std::size_t, std::shared_ptr<Model>>::const_iterator;
		const GLfloat Far_Plane = 124.0f;
		const GLuint ShadowMapSize = 1024;
		glm::vec4 LightVector;
		//void SendToShader(const Shader& sh) = 0; 
		//void Draw(std::pair<const_model_iterator, const_model_iterator> models, glm::vec4) = 0;
		GLint id;
		std::string StrNumLight;
		static inline std::shared_ptr<Shader> shader;
	public:
		Shadow() {};
		Shadow(bool);
		Shadow(const Shadow&);
		Shadow(Shadow&&);
		~Shadow();
};

class PointShadow:public virtual Shadow
{
protected:
	static inline GLuint CubeMapArrayID;
	static inline std::list<GLint> CubeMapIndex;
	void AddBuffer();
public:
	PointShadow() {};
	PointShadow(bool);
	PointShadow(const PointShadow& ) noexcept;
	PointShadow(PointShadow&&) noexcept;
	~PointShadow();
	void SendToShader(const Shader& sh);
	void Draw(std::pair<const_model_iterator, const_model_iterator> models, glm::vec4);
private:
	const GLsizei AmountMatrixes = 6;
	glm::mat4 ShadowProj;
	static inline bool Cleared = false;
	std::array<glm::mat4,6> ViewMatrix;
	GLfloat* pViewMatrix[6];

	const uint16_t MAX_POINT_LIGHTS = 10;
	static inline bool init = false;
};

class DirectionShadow : public virtual Shadow
{
	protected:
		glm::mat4 scale_bias_matrix;
		void AddBuffer();
	private:
		Matrices LightMat;
		void InitOffsetTex(std::size_t size, int64_t samplesU, int64_t samplesV);
		GLuint _3dTexture;
		GLfloat radius;
		glm::vec3 OffsetTexSize;
	public:
		DirectionShadow() {};
		DirectionShadow(bool);
		DirectionShadow(const DirectionShadow&) noexcept;
		DirectionShadow(DirectionShadow&&) noexcept;
		~DirectionShadow();
	void SendToShader(const Shader& sh);
	void Draw(std::pair<const_model_iterator, const_model_iterator> models, glm::vec4);
};