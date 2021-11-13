#pragma once
#include "Headers.h"
#include "FrameBuffer.h"
#include "Model.h"
#include "Transformation.h"
#include <array>

class Shadow:public FrameBuffer
{
private:

protected:
	using const_model_iterator = std::map<std::size_t, std::shared_ptr<Model>>::const_iterator;
	const GLfloat Far_Plane = 64.0f;
	const GLuint ShadowMapSize = 1024;
	const GLfloat borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec4 LightVector;
	GLint id;
	const uint16_t MAX_LIGHTS_ONE_TYPE = 5;
	void AddBuffer();
public:
	Shadow();
	Shadow(const Shadow&);
	~Shadow();
};

class PointShadow:public Shadow
{
	
public:
	PointShadow();
	PointShadow(const PointShadow& ) noexcept;
	~PointShadow();
	//Return FBO
	GLuint AddBuffer(GLint ShadowId);
	void SendToShader(const Shader& sh);
	void Draw(std::pair<const_model_iterator, const_model_iterator> models, glm::vec4);
private:
	static inline GLuint ShadowArrayID;
	static inline std::shared_ptr<Shader> shader;
	static inline bool init = false;

	const GLsizei AmountMatrixes = 6;
	glm::mat4 ShadowProj;
	std::array<glm::mat4,6> ViewMatrix;
	GLfloat* pViewMatrix[6];

	const uint16_t PointShadowBind = 10;
	const std::string_view PointNameShadowUniformTexture = { "PointShadowMaps" };
};

class DirectionShadow : public Shadow
{
	protected:
		static inline glm::mat4 scale_bias_matrix;
		Matrices LightMat;
	private:
		static void InitOffsetTex(std::size_t size, int64_t samplesU, int64_t samplesV);
		static inline GLuint _3dTexture;
		static inline GLfloat radius;
		static inline glm::vec3 OffsetTexSize;
		static inline GLuint ShadowArrayID;
		static inline std::shared_ptr<Shader> shader;
		static inline bool init = false;
		std::string _mStrNumLight;
	public:
		DirectionShadow();
		DirectionShadow(const DirectionShadow&) noexcept;
		//return FBO
		GLuint AddBuffer(GLint ShadowId);
		void SetStrNumLight(const std::string& StrNumLight);
		~DirectionShadow();
	void SendToShader(const Shader& sh);
	void Draw(std::pair<const_model_iterator, const_model_iterator> models, glm::vec4);

	const uint16_t SpotAndDirShadowBind = 11;
	const std::string_view SpotAndDirNameShadowUniformTexture = { "DirLightShadowMaps" };
};