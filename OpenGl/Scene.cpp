#include "Scene.h"

void Scene::Draw()
{
	glClearColor(BackGroundColour.x, BackGroundColour.y, BackGroundColour.z, BackGroundColour.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (const auto& mod : Objects)
		mod.second->Draw(this);
}
void Scene::AddObject(std::string name,std::shared_ptr<Object> obj) 
{
	Objects[std::hash<std::string>{}(name)] = obj;
}

void Scene::AddLight(std::string name, std::shared_ptr<Light> lig)
{
	Lights[std::hash<std::string>{}(name)] = lig;
}

std::shared_ptr<Object> Scene::GetObj(std::string name)
{
	return Objects.find(std::hash<std::string>{}(name))->second;
}
std::shared_ptr<Light> Scene::GetLight(std::string name)
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

std::shared_ptr < glm::mat4> Scene::GetView()
{
	return ViewMatrix;
}
std::shared_ptr < glm::mat4> Scene::GetProj()
{
	return ProjectMatrix;
}
std::shared_ptr < glm::mat4> Scene::GelModel()
{
	return ModelMatrix;
}


glm::vec4 Scene::SetBackGround(glm::vec4 col) 
{
	std::swap(col, BackGroundColour);
	return col;
}


std::unique_ptr<Camera> Scene::SetCam(std::unique_ptr<Camera> Cam)
{
	std::swap(camera,Cam);
	return Cam;
}

Camera* Scene::GetCam() const 
{
	return camera.get();
}