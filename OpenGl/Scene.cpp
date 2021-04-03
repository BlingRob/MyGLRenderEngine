#include "Scene.h"

void Scene::Draw()
{
	std::shared_ptr<Shader> sh;
	static glm::mat4 VP;
	glClearColor(BackGroundColour.x, BackGroundColour.y, BackGroundColour.z, BackGroundColour.w);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	sh = GetShader("Default");
	sh->Use();

		//something
		VP = (*ProjectMatrix) * (*ViewMatrix);

		sh->setMat("transform.view", *ViewMatrix);
		sh->setMat("transform.projection", *ProjectMatrix);
		sh->setMat("transform.VP", VP);
		sh->setVec("viewPos", GetCam()->Position);

		//glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		const auto lights = GetLights();
		for (const auto& el : *lights)
			el.second->SendToShader(*(sh.get()));

	for (auto& mod : Models)
	{
		mod.second->SetShader(GetShader("Default"));
		mod.second->Draw();
	}

	if (SkyBoxSetted)
	{
		glFrontFace(GL_CW);
		glDepthFunc(GL_LEQUAL);
		sh = GetShader("SkyBox");
		sh->Use();
		VP = (*ProjectMatrix) * glm::mat4(glm::mat3(*ViewMatrix));
		sh->setMat("transform.VP", VP);
		SkyBox->Draw(sh);
		glDepthFunc(GL_LESS);
		glFrontFace(GL_CCW);
	}

}
void Scene::AddModel(std::shared_ptr<Model> mod) 
{
	Models[std::hash<std::string>{}(mod->GetName())] = mod;
}

void Scene::AddLight(std::shared_ptr<Light> lig)
{
	Lights[std::hash<std::string>{}(lig->GetName())] = lig;
}

std::shared_ptr<Model> Scene::GetModel(const std::string& name)
{
	return Models.find(std::hash<std::string>{}(name))->second;
}
std::shared_ptr<Light> Scene::GetLight(const std::string& name)
{
	return Lights.find(std::hash<std::string>{}(name))->second;
}

const std::map<std::size_t, std::shared_ptr<Light>>* Scene::GetLights() const
{
	return &Lights;
}

void Scene::SetView(std::shared_ptr < glm::mat4> m)
{
	ViewMatrix = m;
}
void Scene::SetProj(std::shared_ptr < glm::mat4>m)
{
	ProjectMatrix = m;
}
void Scene::SetModel(std::shared_ptr < glm::mat4>m)
{
	ModelMatrix = m;
}

const std::shared_ptr < glm::mat4> Scene::GetView() const
{
	return ViewMatrix;
}
const std::shared_ptr < glm::mat4> Scene::GetProj() const
{
	return ProjectMatrix;
}
const std::shared_ptr < glm::mat4> Scene::GetModel() const
{
	return ModelMatrix;
}

void Scene::SetBackGround(glm::vec4 col) 
{
	BackGroundColour = col;
}

void Scene::AddShader(std::shared_ptr<Shader> sh) 
{
	Shaders[std::hash<std::string>{}(sh->GetName())] = sh;
}
const std::shared_ptr<Shader> Scene::GetShader(const std::string& name) const
{
	return Shaders.find(std::hash<std::string>{}(name))->second;
}


void Scene::SetCam(std::unique_ptr<Camera> Cam)
{
	camera = std::move(Cam);
}

Camera* Scene::GetCam() const 
{
	return camera.get();
}

Scene::Scene(const std::string& path) 
{
	Loader loader(path);

	if (!loader.Is_Load())
		throw(std::string("Problem with scene loading!"));
	if (!loader.Has_Camera())
		//default camera
		SetCam(std::make_unique<Camera>(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
	else
		SetCam(loader.GetCamera());
	std::shared_ptr <Light> light;
	if (!loader.Has_Light())
	{
		//default light
		light = std::make_shared<Light>(0.6f, 0.09f, 0.032f,
										glm::vec3(0.1f, 0.1f, 0.1f),
										glm::vec3(0.8f, 0.8f, 0.8f),
										glm::vec3(1.0f, 1.0f, 1.0f),
										glm::vec3(-1.5f, 10.0f, 0.0f));
		light->SetType(LightTypes::Point);
		light->SetName("Default light");
		AddLight(light);
	}
	else
		while ((light = std::shared_ptr<Light>(loader.GetLight().release())))
			AddLight(light);

	std::shared_ptr<Model> mod;
	while ((mod = std::shared_ptr<Model>(loader.GetModel().release())))
		AddModel(mod);

	SetView(std::make_shared<glm::mat4>(1.0f));
	SetProj(std::make_shared<glm::mat4>(1.0f));
}
Scene::Scene() 
{
};
bool Scene::LoadModels(const std::string& path)
{
	Loader loader(path);
	std::shared_ptr<Model> mod;
	while ((mod = std::shared_ptr<Model>(loader.GetModel().release())))
		AddModel(mod);
	return true;///No finished!
}
bool Scene::LoadLights(const std::string& path)
{
	Loader loader(path);
	std::shared_ptr <Light> light;

	while ((light = std::shared_ptr<Light>(loader.GetLight().release())))
		AddLight(light);
	return true;//No finished!
}

void Scene::SetBackGround(std::vector<std::string_view> paths) 
{
	SkyBox = Loader::LoadSkyBox(paths);
	std::shared_ptr<Shader> shaders = std::make_shared<Shader>("..\\Shaders\\SkyBox.vert", "..\\Shaders\\SkyBox.frag");
	shaders->SetName("SkyBox");
	AddShader(shaders);
	SkyBoxSetted = true;
}