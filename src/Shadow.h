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
	static void InitOffsetTex(std::size_t size, int64_t samplesU, int64_t samplesV);
	static inline GLuint CubeMapArrayID;
	static inline glm::mat4 scale_bias_matrix, ShadowProj;
	static inline std::list<GLint> CubeMapIndex;
	static inline const GLfloat Far_Plane = 124.0f;

	GLint id;
public:
	Shadow();
	~Shadow();
	void AddShadowMap();
	void AddCubeShadowMap();
	void SendToShader(const Shader& sh);
	void Draw(std::pair<const_model_iterator, const_model_iterator> models,glm::vec4);
	static const GLuint ShadowMapSize = 1024,
		ShadowMapDepth = 124;
private:
	const GLsizei AmountMatrixes = 6, SizeMatrix = 4;
	static inline GLuint _3dTexture;
	static inline GLfloat radius;
	static inline glm::vec3 OffsetTexSize;
	static inline bool Cleared = false;
	std::array<glm::mat4,6> ViewMatrix;
	GLfloat* pViewMatrix[6];
	glm::vec4 LightPos;

};