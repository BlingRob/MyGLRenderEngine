#include "Scene.h"

void Scene::Draw()
{
	std::shared_ptr<Shader> sh;
	if (!SkyBoxSetted)
		glClearColor(BackGroundColour.x, BackGroundColour.y, BackGroundColour.z, BackGroundColour.w);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const auto lights = GetLights();
	ShadowDraw();
	for (auto& mod : Models)
	{
		sh = mod.second->GetShader();
		sh->Use();
		ShadowMap->SendToShader(sh, "shadowMap");
		matrs.SendToShader(*sh);//Needing use uniform buffer
		sh->setMat("ShadowMatrix", scale_bias_matrix * (*LightMat.Projection) * (*LightMat.View));
		sh->setVec("viewPos", GetCam()->Position);//for 
		for (const auto& el : *lights)//this data
			el.second->SendToShader(*sh);//really important
		mod.second->Draw(nullptr);
	}

	if (SkyBoxSetted)
	{
		glFrontFace(GL_CW);
		glDepthFunc(GL_LEQUAL);
		sh = GetShader("SkyBox");
		sh->Use();
		sh->setMat("transform.PV", (*matrs.Projection * glm::mat4(glm::mat3(*matrs.View))));
		SkyBox->Draw(sh);
		glDepthFunc(GL_LESS);
		glFrontFace(GL_CCW);
	}

}

void Scene::ShadowDraw()
{
	static GLfloat OldView[4];
	static std::shared_ptr<Shader> sh(ShadowMap->GetShader());
	ShadowMap->AttachBuffer();
	glGetFloatv(GL_VIEWPORT, OldView);
	glViewport(0, 0, ShadowMapSize, ShadowMapSize);
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0f, 4.0f);
	//glCullFace(GL_FRONT);
	if(std::shared_ptr<Light> lig;lig = GetLight("Point_Orientation"))
		LightMat.View = std::make_shared<glm::mat4>(glm::lookAt(lig->GetPos(),
													glm::vec3(0.0f, 0.0f, 0.0f),
													glm::vec3(0.0f, 1.0f, 0.0f)));
	sh->Use();
	LightMat.SendToShader(*sh);
	for (auto& mod : Models)
		mod.second->Draw(sh);
	ShadowMap->DetachBuffer();
	glDisable(GL_POLYGON_OFFSET_FILL);
	//glCullFace(GL_BACK);
	glViewport(0, 0, OldView[2], OldView[3]);
}

void Scene::AddModel(std::shared_ptr<Model> mod) 
{
	Models[std::hash<std::string_view>{}(mod->GetName())] = mod;
}

void Scene::AddLight(std::shared_ptr<Light> lig)
{
	Lights[std::hash<std::string_view>{}(lig->GetName())] = lig;
}

std::shared_ptr<Model> Scene::GetModel(std::string_view  name)
{
	if (auto iter = Models.find(std::hash<std::string_view >{}(name)); iter != Models.end())
		return iter->second;
	return nullptr;
}
std::shared_ptr<Light> Scene::GetLight(std::string_view  name)
{
	if (auto iter = Lights.find(std::hash<std::string_view >{}(name)); iter != Lights.end())
		return iter->second;
	return nullptr;
}
const std::shared_ptr<Shader> Scene::GetShader(std::string_view name) const
{
	if (auto iter = Shaders.find(std::hash<std::string_view>{}(name)); iter != Shaders.end())
		return iter->second;
	return nullptr;
}
const glm::vec4 Scene::GetBackGround() const 
{
	return BackGroundColour;
}

const std::map<std::size_t, std::shared_ptr<Light>>* Scene::GetLights() const
{
	return &Lights;
}

void Scene::SetBackGround(glm::vec4 col) 
{
	BackGroundColour = col;
}

void Scene::AddShader(std::shared_ptr<Shader> sh) 
{
	Shaders[std::hash<std::string_view>{}(sh->GetName())] = sh;
}

void Scene::SetCam(std::unique_ptr<Camera> Cam)
{
	camera = std::move(Cam);
}

Camera* Scene::GetCam() const 
{
	return camera.get();
}


Scene::Scene() 
{
	//default background
	BackGroundColour = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	//default shaders
	std::shared_ptr<Shader> shader = std::make_shared<Shader>("../Shaders/Base.vert", "../Shaders/Base.frag");
	shader->SetName("Default");
	AddShader(shader);
	shader = std::make_shared<Shader>("../Shaders/Light.vert", "../Shaders/Light.frag");
	shader->SetName("PointLight");
	AddShader(shader);
	if (DefaultPointLightModel)
		DefaultPointLightModel->SetShader(shader);
	shader = std::make_shared<Shader>("../Shaders/SkyBox.vert", "../Shaders/SkyBox.frag");
	shader->SetName("SkyBox");
	AddShader(shader);
	SetCam(std::make_unique<Camera>(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
	SkyBox = DefaultSkyBox;
	SkyBoxSetted = true;
	
	ShadowMap = std::make_unique<ShadowMapBuffer>(ShadowMapSize, ShadowMapSize);
	ShadowMap->AddFrameBuffer(BufferType::Depth);
	scale_bias_matrix = glm::mat4(0.5f);
	scale_bias_matrix[3] = glm::vec4(0.5f);
	scale_bias_matrix[3][3] = 1;
	//LightMat.Projection = std::make_shared<glm::mat4>(glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, 0.1f, 30.0f));
	LightMat.Projection = std::make_shared<glm::mat4>(glm::perspective(180.0f, 1.0f, 0.1f, 30.0f));
};
/*
bool Scene::LoadModels(const std::string& path)
{
	Loader loader(path);
	std::shared_ptr<Model> mod;
	while ((mod = std::shared_ptr<Model>(loader.GetModel().release())))
		AddModel(mod);
	return true;///No finished!
}*/

void Scene::SetBackGround(std::shared_ptr<Node> box)
{
	SkyBox = box;
	if (GetShader("SkyBox") == nullptr)
	{

	}
	SkyBoxSetted = true;
}

Scene_Information Scene::GetInfo()
{
	return std::move(Scene_Information({ Models.size(), Lights.size(), Shaders.size()}));
}