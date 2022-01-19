#pragma once
#include "FrameBuffer.h"
#include "Model.h"
#include "Transformation.h"
#include <array>
#include <map>

class Shadow:public FrameBuffer
{
public:
	using const_model_iterator = std::map<std::size_t, std::shared_ptr<Model>>::const_iterator;
	using Models = std::pair<const_model_iterator, const_model_iterator>;
	Shadow();
	Shadow(const Shadow&) = default;
	virtual void Draw(Models& models, glm::vec4&) = 0;
	virtual void SendToShader(const Shader & sh) = 0;
	GLuint GetFBO() const;
	~Shadow();
	GLint id;
private:

protected:
	const GLfloat Far_Plane = 64.0f;
	const GLuint ShadowMapSize = 1024;
	const GLfloat borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec4 LightVector;
	const uint16_t MAX_LIGHTS_ONE_TYPE = 5;
};

class PointShadow:public Shadow
{
public:
	PointShadow();
	PointShadow(const PointShadow& ) noexcept = default;
	~PointShadow();
	void SendToShader(const Shader& sh) override;
	void Draw(Models&, glm::vec4&) override;
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

	static inline std::list<GLint> ShadowIdxes;
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
		DirectionShadow(const DirectionShadow&) noexcept = default;
		~DirectionShadow();
	void SendToShader(const Shader& sh) override;
	void Draw(Models& models, glm::vec4&) override;

	const uint16_t SpotAndDirShadowBind = 11;
	const std::string_view SpotAndDirNameShadowUniformTexture = { "DirLightShadowMaps" };

	static inline std::list<GLint> ShadowIdxes;
};
