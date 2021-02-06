#include "Scene.h"

void Scene::Draw()
{
	glClearColor(BackGroundColour.x, BackGroundColour.y, BackGroundColour.z, BackGroundColour.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto mod : Objects)
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

void Scene::SetView(std::shared_ptr < glm::mat4> m)
{
	matrixs[1] = m;
}
void Scene::SetProj(std::shared_ptr < glm::mat4>m)
{
	matrixs[2] = m;
}
void Scene::SetModel(std::shared_ptr < glm::mat4>m)
{
	matrixs[0] = m;
}

glm::vec4 Scene::SetBackGround(glm::vec4 col) 
{
	std::swap(col, BackGroundColour);
	return col;
}