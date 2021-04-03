#pragma once
#include "Light.h"
#include "Ñamera.h"
#include "Node.h"
#include "Model.h"
#include "Loader.h"

class Scene
{
	std::unique_ptr<Camera> camera;
	std::map<std::size_t, std::shared_ptr<Model>> Models;
	std::map<std::size_t, std::shared_ptr<Light>> Lights;
	std::map<std::size_t, std::shared_ptr<Shader>> Shaders;

	std::shared_ptr<glm::mat4> ViewMatrix,
							   ProjectMatrix,
							   ModelMatrix;

	glm::vec4 BackGroundColour;
	bool SkyBoxSetted = false;
	std::unique_ptr<Node> SkyBox;

	public:
	
	void SetBackGround(glm::vec4 col);
	void SetBackGround(std::vector<std::string_view> paths);

	void AddModel(std::shared_ptr<Model> obj);
	void AddLight(std::shared_ptr<Light> lig);

	void AddShader(std::shared_ptr<Shader> sh);
	const std::shared_ptr<Shader> GetShader(const std::string& name) const;

	std::shared_ptr<Model> GetModel(const std::string& name);
	std::shared_ptr<Light> GetLight(const std::string& name);
	const std::map<std::size_t, std::shared_ptr<Light>>* GetLights() const;

	void Draw();

	void SetView(std::shared_ptr < glm::mat4>);
	void SetProj(std::shared_ptr < glm::mat4>);
	void SetModel(std::shared_ptr < glm::mat4>);

	const std::shared_ptr < glm::mat4> GetView() const;
	const std::shared_ptr < glm::mat4> GetProj() const;
	const std::shared_ptr < glm::mat4> GetModel() const;

	void SetCam(std::unique_ptr<Camera>);
	Camera* GetCam() const;

	Scene(const std::string& path);
	Scene();
	bool LoadModels(const std::string& path);
	bool LoadLights(const std::string& path);
};


