#pragma once
#include "Headers.h"
#include "Model.h"
#include "camera.h"
//#include "Object.h"
#include "Light.h"

struct Object;
class Scene
{
	std::unique_ptr<Camera> camera;
	public:
	std::map<std::size_t, std::shared_ptr<Object>> Objects;
	std::map<std::size_t, std::shared_ptr<Light>> Lights;
	std::vector<std::shared_ptr<glm::mat4>> matrixs;
	glm::vec4 BackGroundColour;
	public:
		Scene() { matrixs.resize(5); }
		glm::vec4 SetBackGround(glm::vec4 col);

		void AddObject(std::string name, std::shared_ptr<Object> obj);
		void AddLight(std::string name, std::shared_ptr<Light> lig);
		std::shared_ptr<Object> GetObj(std::string name);
		std::shared_ptr<Light> GetLight(std::string name);

		void Draw();

		void SetView(std::shared_ptr < glm::mat4>);
		void SetProj(std::shared_ptr < glm::mat4>);
		void SetModel(std::shared_ptr < glm::mat4>);
};

struct Object
{
	Object(std::function<void(Scene*)> f) : Draw(f) {}
	std::function< void(Scene*) > Draw;
};

