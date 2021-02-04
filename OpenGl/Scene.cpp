#include "Scene.h"

void Scene::Draw()
{
	glClearColor(BackGroundColour.x, BackGroundColour.y, BackGroundColour.z, BackGroundColour.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto mod : Objects)
		mod->Draw(lights, matrixs);
}
void Scene::AddObject(std::shared_ptr<Object> obj) 
{
	Objects.push_back(obj);
}

void Scene::AddLight(std::shared_ptr<Light> lig)
{
	lights.push_back(lig);
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