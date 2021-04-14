#pragma once
#include "Headers.h"
#include "Light.h"
#include "�amera.h"
#include "Node.h"
#include "Model.h"
#include "Transformation.h"

class Scene
{
	std::unique_ptr<Camera> camera;
	std::map<std::size_t, std::shared_ptr<Model>> Models;
	std::map<std::size_t, std::shared_ptr<Light>> Lights;
	std::map<std::size_t, std::shared_ptr<Shader>> Shaders;

	glm::vec4 BackGroundColour;
	bool SkyBoxSetted = false;
	std::unique_ptr<Node> SkyBox;

	public:
	
	void SetBackGround(glm::vec4 col);
	void SetBackGround(std::unique_ptr<Node> box);
	std::unique_ptr<Node> RetSkyBoxGround();

	void AddModel(std::shared_ptr<Model> obj);
	void AddLight(std::shared_ptr<Light> lig);
	void AddShader(std::shared_ptr<Shader> sh);

	const std::shared_ptr<Shader> GetShader(std::string_view name) const;
	std::shared_ptr<Model> GetModel(std::string_view  name);
	std::shared_ptr<Light> GetLight(std::string_view  name);
	/*Return const pointer to std::map conteiner with pairs (NameHash-pointerToLight)*/
	const std::map<std::size_t, std::shared_ptr<Light>>* GetLights() const;

	void Draw();

	Matrices matrs;

	void SetCam(std::unique_ptr<Camera>);
	Camera* GetCam() const;

	//Scene(const std::string& path);
	Scene();
	//bool LoadModels(const std::string& path);
};

