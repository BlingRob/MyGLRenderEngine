#include "../Headers/Scene.h"

void Scene::Draw()
{
	std::shared_ptr<Shader> sh;
	//GLenum err = glGetError();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Light::ClearBuffers();
	for (const auto& lig : Lights) 
		lig.second->DrawShadows(GetModels());
	for (auto& mod : Models)
	{
		sh = mod.second->GetShader();
		sh->Use();
		sh->setScal("NumLights", Lights.size());
		matrs->SendToShader(*sh);//Needing use uniform buffer
		sh->setVec("viewPos", GetCam()->Position);//for 
		for (const auto& lig : Lights)
			lig.second->SendToShader(*sh);//really important
		mod.second->Draw(nullptr);
	}	
	if (SkyBoxSetted)
	{
		glFrontFace(GL_CW);
		glDepthFunc(GL_LEQUAL);
		sh = GetShader("SkyBox");
		sh->Use();
		sh->setMat("transform.PV", (*matrs->Projection * glm::mat4(glm::mat3(*matrs->View))));
		SkyBox->Draw(sh.get());
		glDepthFunc(GL_LESS);
		glFrontFace(GL_CCW);
	}
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

const std::pair<Scene::LIt, Scene::LIt> Scene::GetLights() const
{
	return make_pair(Lights.cbegin(), Lights.cend());
}
const std::pair<Scene::MIt, Scene::MIt> Scene::GetModels() const
{
	return make_pair(Models.cbegin(), Models.cend());
}

void Scene::SetBackGround(glm::vec4 col) 
{
	BackGroundColour = col;
	glClearColor(BackGroundColour.x, BackGroundColour.y, BackGroundColour.z, BackGroundColour.w);
}

void Scene::AddShader(std::shared_ptr<Shader> sh) 
{
	Shaders[std::hash<std::string_view>{}(sh->GetName())] = sh;
}

void Scene::SetCam(std::shared_ptr<Camera> Cam)
{
	camera = std::move(Cam);
}

std::shared_ptr<Camera> Scene::GetCam() const
{
	return matrs->cam;
}


Scene::Scene(std::shared_ptr <Position_Controller> contr, std::function<void()> fun)
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
	//SetCam(std::make_unique<Camera>(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
	SkyBox = DefaultSkyBox;
	SkyBoxSetted = true;
	SetController(contr);
	DeleterAssimpScene = fun;
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
void Scene::SetController(std::shared_ptr <Position_Controller> contr) 
{
	if (camera)
		contr->cam = camera;
	matrs = contr;
}

Scene::~Scene() 
{
	//Mesh::GlobalTextures.clear();
	DeleterAssimpScene();
}