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
		for (const auto& lig : Lights)
			lig.second->SendToShader(*sh);
		mod.second->Draw();
	}	

	for (const auto& lig : Lights)
	{
		if (lig.second->GetType() == LightTypes::Point)
		{
			sh = lig.second->GetModel()->GetShader();
			sh->Use();
			matrs->SendToShader(*sh);
			lig.second->Draw();
		}
	}

	if (SkyBoxSetted)
	{
		glFrontFace(GL_CW);
		glDepthFunc(GL_LEQUAL);
		sh = ShadersBank::GetShader("SkyBox");
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

std::shared_ptr<Model> Scene::GetModel(std::string_view name)
{
	if (auto iter = Models.find(std::hash<std::string_view>{}(name)); iter != Models.end())
		return iter->second;
	return nullptr;
}
std::shared_ptr<Light> Scene::GetLight(std::string_view name)
{
	if (auto iter = Lights.find(std::hash<std::string_view>{}(name)); iter != Lights.end())
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

void Scene::SetBackGround(const glm::vec4& col) 
{
	BackGroundColour = col;
	glClearColor(BackGroundColour.x, BackGroundColour.y, BackGroundColour.z, BackGroundColour.w);
}

void Scene::SetCam(std::shared_ptr<Camera> Cam)
{
	camera = std::move(Cam);
}

std::shared_ptr<Camera> Scene::GetCam() const
{
	return matrs->cam;
}


Scene::Scene(std::shared_ptr <Position_Controller> contr)
{
	//default background
	BackGroundColour = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	if (DefaultSkyBox)
	{
		SkyBox = DefaultSkyBox;
		SkyBoxSetted = true;
	}
	SetController(contr);
};

void Scene::SetBackGround(std::shared_ptr<Node> box)
{
	SkyBox = box;
	if (ShadersBank::GetShader("SkyBox") == nullptr)
	{

	}
	SkyBoxSetted = true;
}

Scene_Information Scene::GetInfo()
{
	return Scene_Information({ Models.size(), Lights.size(), ShadersBank::Size()});
}
void Scene::SetController(std::shared_ptr <Position_Controller> contr) 
{
	if (camera)
		contr->cam = camera;
	matrs = contr;
}

Scene::~Scene() 
{
}