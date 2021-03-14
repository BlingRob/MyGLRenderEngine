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
	std::map<std::size_t, std::shared_ptr<Object>> Objects;
	std::map<std::size_t, std::shared_ptr<Light>> Lights;

	std::shared_ptr<glm::mat4> ViewMatrix,
							   ProjectMatrix,
							   ModelMatrix;

	glm::vec4 BackGroundColour;

	public:
	
	glm::vec4 SetBackGround(glm::vec4 col);

	void AddObject(std::string name, std::shared_ptr<Object> obj);
	void AddLight(std::string name, std::shared_ptr<Light> lig);
	std::shared_ptr<Object> GetObj(std::string name);
	std::shared_ptr<Light> GetLight(std::string name);
	const std::map<std::size_t, std::shared_ptr<Light>>* GetLights() const;

	void Draw();

	void SetView(std::shared_ptr < glm::mat4>);
	void SetProj(std::shared_ptr < glm::mat4>);
	void SetModel(std::shared_ptr < glm::mat4>);

	std::shared_ptr < glm::mat4> GetView() const;
	std::shared_ptr < glm::mat4> GetProj() const;
	std::shared_ptr < glm::mat4> GelModel() const;

	std::unique_ptr<Camera> SetCam(std::unique_ptr<Camera>);
	Camera* GetCam() const;
};

struct Object
{
	Object(std::function<void(Scene*)> f) : Draw(f) {}
	std::function< void(Scene*) > Draw;
};

