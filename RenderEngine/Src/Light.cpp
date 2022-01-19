#include "../Headers/Light.h"

BLight::~BLight()
{
	if(id != -1)
		LightIdxes.insert(std::lower_bound(LightIdxes.begin(), LightIdxes.end(), id), id);
}

void BLight::SendToShader(const Shader& shader)
{
	shader.setVec((_mStrNumLight + "ambient"), _mAmbient);
	shader.setVec((_mStrNumLight + "diffuse"), _mDiffuse);
	shader.setVec((_mStrNumLight + "specular"), _mSpecular);
	shader.setVec((_mStrNumLight + "clq"), _mCLQ);
	shader.setVec((_mStrNumLight + "LightPositions"), LightVec);
	shader.setScal((_mStrNumLight + "index"), _pShadow->id);
	_pShadow->SendToShader(shader);
}

void BLight::DrawShadow(Shadow::Models& models)
{
	_pShadow->Draw(models,LightVec);
}

LightTypes BLight::GetType()
{
	return LightTypes::None;
}

void BLight::SetVec(const glm::vec4& vec)
{
	LightVec = vec;
}

glm::vec4 BLight::GetVec() const
{
	return LightVec;
}

glm::vec3 BLight::GetAmbient() const
{
	return _mAmbient;
}

glm::vec3 BLight::GetDiffuse() const
{
	return _mDiffuse;
}

glm::vec3 BLight::GetSpecular() const
{
	return _mSpecular;
}

glm::vec3 BLight::GetAttenuation() const
{
	return _mCLQ;
}

const Shadow* BLight::GetShadow() const
{
	return _pShadow.get();
}

void BLight::SetAmbient(const glm::vec3& amb)
{
	_mAmbient = amb;
}

void BLight::SetSpecular(const glm::vec3& spec)
{
	_mSpecular = spec;
}

void BLight::SetDiffuse(const glm::vec3& dif)
{
	_mDiffuse = dif;
}

void BLight::SetAttenuation(const glm::vec3& CLQ)
{
	_mCLQ = CLQ;
}

DirectionalLight::DirectionalLight(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s, const glm::vec3& CLQ, const glm::vec3& dir) :
BLight(a, d, s, CLQ, glm::vec4(dir, 0.0f))
{
	_pShadow = std::make_shared<DirectionShadow>();
}

void DirectionalLight::ChangeDirection(const glm::vec3& NewDir)
{
	SetVec(glm::vec4(NewDir, 0.0f));
}

glm::vec3 DirectionalLight::GetDir() const
{
	return GetVec();
}

PointLight::PointLight(const glm::vec3& a, const glm::vec3& d, const glm::vec3& s, const glm::vec3& CLQ, const glm::vec3& p):
BLight(a, d, s, CLQ, glm::vec4(p, 1.0f))
{
	_pShadow = std::make_shared<PointShadow>();
}

void PointLight::SetPos(const glm::vec3& p)
{
	SetVec(glm::vec4(p, 1.0f));
}

glm::vec3 PointLight::GetPos() const
{
	return GetVec();
}

std::shared_ptr<Model> PointLight::GetModel() 
{
	return _pPointLightModel;
}

void PointLight::SetModel(std::shared_ptr<Model> mod) 
{
	_pPointLightModel = std::move(mod);
	_pPointLightModel->Translate(GetPos());
}

void PointLight::Draw() 
{
	_pPointLightModel->GetShader()->setVec("Colour", BLight::GetAmbient());
	_pPointLightModel->Draw();
}

void SpotLight::SendToShader(const Shader& shader)
{
	BLight::SendToShader(shader);
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

Light_Interface::Light_Interface(BLight* _plight):_pLight(_plight)
{
	_pPoint = dynamic_cast<PointLight*>(_pLight);
	_pDir = dynamic_cast<DirectionalLight*>(_pLight);
	_pSpot = dynamic_cast<SpotLight*>(_pLight);
}

glm::vec3 Light_Interface::GetAmbient() const 
{
	return _pLight->GetAmbient();
}

glm::vec3 Light_Interface::GetDiffuse() const 
{
	return _pLight->GetDiffuse();
}

glm::vec3 Light_Interface::GetSpecular() const 
{
	return _pLight->GetSpecular();
}

glm::vec3 Light_Interface::GetAttenuation() const 
{
	return _pLight->GetAttenuation();
}

void Light_Interface::SetAmbient(const glm::vec3& amb) 
{
	_pLight->SetAmbient(amb);
}

void Light_Interface::SetDiffuse(const glm::vec3& diff)
{
	_pLight->SetAmbient(diff);
}

void Light_Interface::SetSpecular(const glm::vec3& spec) 
{
	_pLight->SetSpecular(spec);
}
void Light_Interface::SetAttenuation(const glm::vec3& att)
{
	_pLight->SetAttenuation(att);
}

std::pair<float, float> Light_Interface::GetAngels() const
{
	return (_pSpot) ? _pSpot->GetAngels() : std::make_pair(0.f,0.f);
}

void Light_Interface::SetAngels(const std::pair<float, float>& NewAngels)
{
	if(_pSpot)
		_pSpot->SetAngels(NewAngels);
}

/*Set new pos*/
void Light_Interface::SetPos(const glm::vec3& p)
{
	if(_pPoint)
        _pPoint->SetPos(p);
}

/*Return current pos*/
glm::vec3 Light_Interface::GetPos() const
{
	return (_pPoint) ? _pPoint->GetPos() : glm::vec3(0.0f);
}

void Light_Interface::Draw() 
{
	if (_pPoint)
		_pPoint->Draw();
}

std::shared_ptr<Model> Light_Interface::GetModel() 
{
	return (_pPoint) ? _pPoint->GetModel() : nullptr;
}

void Light_Interface::SetModel(std::shared_ptr<Model> mod) 
{
	if(_pPoint)
		_pPoint->SetModel(std::move(mod));
}

void Light_Interface::ChangeDirection(const glm::vec3& NewDir)
{
	if(_pDir)
        _pDir->ChangeDirection(NewDir);
}

glm::vec3 Light_Interface::GetDir() const
{
	return (_pDir) ? _pDir->GetDir() : glm::vec3(0.0f);
}

Light_Interface::~Light_Interface()
{
	_pDir = nullptr;
	_pPoint = nullptr;
	_pDir = nullptr;
	delete _pLight;
}

LightTypes Light::GetType() const
{
	return Light_Interface::_pLight->GetType();
}

void Light::SendToShader(const Shader& shader)
{
	if (Initialized)
		Light_Interface::_pLight->SendToShader(shader);
}

void Light::DrawShadows(Shadow::Models models)
{
	if (Initialized)
		Light_Interface::_pLight->DrawShadow(models);
}

void Light::ClearBuffers()
{
	const GLfloat depthVal = 1.0f;
	if (!_mFBO.empty())
		for(const auto& fbo:_mFBO)
			glClearNamedFramebufferfv(fbo, GL_DEPTH, 0, &depthVal);
}

Light::~Light()
{
	if (Initialized)
		_mFBO.remove(Light_Interface::_pLight->GetShadow()->GetFBO());
}
