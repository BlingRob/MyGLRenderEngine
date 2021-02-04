#pragma once
#include "Headers.h"
#include "Model.h"
#include "camera.h"
#include "Object.h"
#include "Light.h"

class Scene
{
	std::unique_ptr<Camera> camera;
	std::list<std::shared_ptr<Object>> Objects;
	std::list<std::shared_ptr<Light>> lights;
	std::vector<std::shared_ptr<glm::mat4>> matrixs;
	glm::vec4 BackGroundColour;
	public:
		Scene() { matrixs.resize(5); }
		glm::vec4 SetBackGround(glm::vec4 col);
		void AddObject(std::shared_ptr<Object> obj);
		void AddLight(std::shared_ptr<Light> lig);
		void Draw();
		void SetView(std::shared_ptr < glm::mat4>);
		void SetProj(std::shared_ptr < glm::mat4>);
		void SetModel(std::shared_ptr < glm::mat4>);
};

