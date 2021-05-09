#include "Light.h"

void BLight::SendToShader(const Shader& shader)
{
	shader.setVec((lightStr + "ambient"),ambient);
	shader.setVec((lightStr + "diffuse"),diffuse);
	shader.setVec((lightStr + "specular"), specular);
}

glm::vec3 BLight::GetAmbient() const 
{
	return ambient;
}
glm::vec3 BLight::GetDiffuse() const
{
	return diffuse;
}
glm::vec3 BLight::GetSpecular() const
{
	return specular;
}
void BLight::SetAmbient(const glm::vec3& amb)
{
	ambient = amb;
}
void BLight::SetSpecular(const glm::vec3& spec)
{
	specular = spec;
}
void BLight::SetDiffuse(const glm::vec3& dif)
{
	diffuse = dif;
}

DirectionalLight::DirectionalLight(glm::vec3 a, glm::vec3 d, glm::vec3 s, glm::vec3 dir) :
BLight(a, d, s), direction(dir)
{
	//ShadowMap = std::make_unique<ShadowMapBuffer>(ShadowMapSize, ShadowMapSize);
	//ShadowMap->AddFrameBuffer(BufferType::Depth);
	//scale_bias_matrix = glm::mat4(0.5f);
	//scale_bias_matrix[3] = glm::vec4(0.5f);
	//scale_bias_matrix[3][3] = 1;
	//LightMat.Projection = std::make_shared<glm::mat4>(glm::ortho(static_cast<float>(ShadowMapSize), static_cast<float>(ShadowMapSize),0.1f,100.0f));
}

void DirectionalLight::SendToShader(const Shader& shader)
{
	BLight::SendToShader(shader);
	shader.setVec("DirLight.direction", direction);
	//ShadowMap->SendToShader(shader, "shadowMap");
	//shader.setMat("DirLight.ShadowMatrix", scale_bias_matrix * (*LightMat.Projection) * (*LightMat.View));
}

/*
void DirectionalLight::ShadowDraw(std::pair<const_model_iterator, const_model_iterator> Models , uint16_t NumLights)
{
	static GLfloat OldView[4];
	static std::shared_ptr<Shader> sh(ShadowMap->GetShader());
	ShadowMap->AttachBuffer();
	glGetFloatv(GL_VIEWPORT, OldView);
	glViewport(0, 0, ShadowMapSize, ShadowMapSize);
	//glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.5f, 10.0f);
	glCullFace(GL_FRONT);
	LightMat.View = std::make_shared<glm::mat4>(glm::lookAt(direction,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)));
	sh->Use();
	LightMat.SendToShader(*sh);
	for (auto& it = Models.first; it != Models.second; ++it)
		it->second->Draw(sh);
	ShadowMap->DetachBuffer();
	glDisable(GL_POLYGON_OFFSET_FILL);
	glCullFace(GL_BACK);
	glViewport(0, 0, OldView[2], OldView[3]);
}*/


void DirectionalLight::ChangeDirection(const glm::vec3& NewDir)
{
	direction = NewDir;
}
void PointLight::SetAttenuation(const glm::vec3& CLQ)
{
	constant = CLQ.x;
	linear = CLQ.y;
	quadratic = CLQ.z;
}
glm::vec3 PointLight::GetAttenuation() const
{
	return glm::vec3(constant, linear, quadratic);
}

void PointLight::SendToShader(const Shader& shader)
{
	BLight::SendToShader(shader);
	char buffer[32];
	snprintf(buffer, 32, "LightPositions[%d]", Shadow::id);

	shader.setVec(buffer, glm::vec4(GetPos(),1.0f));
	shader.setScal((lightStr + "constant"), constant);
	shader.setScal((lightStr + "linear"), linear);
	shader.setScal((lightStr + "quadratic"), quadratic);
	shader.setScal((lightStr + "index"), Shadow::id);
	Shadow::SendToShader(shader);
}
PointLight::PointLight(glm::vec3 a, glm::vec3 d, glm::vec3 s, glm::vec3 p, float c, float l, float q):
BLight(a, d, s)
{
	constant = c;
	linear = l;
	quadratic = q;

	SetPos(p);
	Shadow::AddCubeShadowMap();
}

/*
void PointLight::ShadowDraw(std::pair<const_model_iterator, const_model_iterator> Models, uint16_t NumLights)
{
	static GLfloat OldView[4];
	static std::shared_ptr<Shader> sh(ShadowMap->GetShader());
	ShadowMap->AttachBuffer();
	glGetFloatv(GL_VIEWPORT, OldView);
	glViewport(0, 0, ShadowMapSize, ShadowMapSize);
	//glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.5f, 10.0f);
	glCullFace(GL_FRONT);
	LightMat.View = std::make_shared<glm::mat4>(glm::lookAt(GetPos(),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)));
	sh->Use();
	LightMat.SendToShader(*sh);
	for (auto& it = Models.first; it != Models.second; ++it)
		it->second->Draw(sh);
	ShadowMap->DetachBuffer();
	glDisable(GL_POLYGON_OFFSET_FILL);
	glCullFace(GL_BACK);
	glViewport(0, 0, OldView[2], OldView[3]);
}*/

void PointLight::SetPos(const glm::vec3& p)
{
	(*tr.Get())[3] = glm::vec4(p,1.0f);
}

glm::vec3 PointLight::GetPos() const
{
	return (*tr.Get())[3];
}

void SpotLight::SendToShader(const Shader& shader)
{
	PointLight::SendToShader(shader);
	shader.setScal("light.Theta", Theta);
	shader.setScal("light.Alpha", Alpha);
}

std::pair<float, float> SpotLight::GetAngels() const
{
	return std::make_pair(Theta,Alpha);
}

void SpotLight::SetAngels(const std::pair<float, float>& NewAngels) 
{
	Theta = NewAngels.first;
	Alpha = NewAngels.second;
}

void Light::SetType(LightTypes type) 
{
	Type = type;
}
LightTypes Light::GetType() const
{
	return Type;
}
void Light::SendToShader(const Shader& shader)
{
	switch (Type) 
	{
		case LightTypes::Directional:
			DirectionalLight::SendToShader(shader);
		break;
		case LightTypes::Spot:
			SpotLight::SendToShader(shader);
		break;
		case LightTypes::Point:
		default:
			PointLight::SendToShader(shader);
		break;
	}
}

bool Light::Init() 
{
	glCreateTextures(GL_TEXTURE_CUBE_MAP_ARRAY, 1, &Shadow::CubeMapArrayID);

	glTextureStorage3D(Shadow::CubeMapArrayID, 1, GL_DEPTH_COMPONENT24, Shadow::ShadowMapSize, Shadow::ShadowMapSize, PointLight::MAX_POINT_LIGHTS * 6);
	glTextureParameteri(Shadow::CubeMapArrayID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(Shadow::CubeMapArrayID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(Shadow::CubeMapArrayID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTextureParameteri(Shadow::CubeMapArrayID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(Shadow::CubeMapArrayID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glTextureParameteri(Shadow::CubeMapArrayID, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	//glTextureParameteri(Shadow::CubeMapArrayID, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

	Shadow::scale_bias_matrix = glm::mat4(0.5f);
	Shadow::scale_bias_matrix[3] = glm::vec4(0.5f);
	Shadow::scale_bias_matrix[3][3] = 1;

	Shadow::CubeMapIndex.resize(PointLight::MAX_POINT_LIGHTS);
	std::iota(Shadow::CubeMapIndex.begin(), Shadow::CubeMapIndex.end(), 0);
	//for (uint16_t i = 0; i < PointLight::MAX_POINT_LIGHTS; ++i)
	//	Shadow::CubeMapIndex.push_back(i);
	Shadow::ShadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, Shadow::Far_Plane);
	Shadow::InitOffsetTex(16, 8, 8);

	return true;
}

void Light::DrawShadows(std::pair<Shadow::const_model_iterator, const_model_iterator> models) 
{
	switch (Type)
	{
	case LightTypes::Directional:
		//DirectionalLight::SendToShader(shader);
		break;
	case LightTypes::Spot:
		//SpotLight::SendToShader(shader);
		break;
	case LightTypes::Point:
	default:
		
		Shadow::Draw(models, glm::vec4(PointLight::GetPos(), 1.0f));
		break;
	}
}