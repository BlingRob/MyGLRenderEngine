#pragma once
#include <functional>
#include <map>
#include "Light.h"
#include "Camera.h"
#include "Model.h"
#include "Transformation.h"
#include "FrameBuffer.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

struct Scene_Information
{
	std::size_t Amount_models,
				Amount_lights,
				Amount_shaders;
};

class Scene
{
	std::function<void()> DeleterAssimpScene;
	std::shared_ptr<Camera> camera;
	std::shared_ptr <Position_Controller> matrs;

	std::map<std::size_t, std::shared_ptr<Model>> Models;
	std::map<std::size_t, std::shared_ptr<Light>> Lights;
	std::map<std::size_t, std::shared_ptr<Shader>> Shaders;

	glm::vec4 BackGroundColour;
	std::shared_ptr<Node> SkyBox;

	public:
	using LIt = std::map<std::size_t, std::shared_ptr<Light>>::const_iterator; // Iterator on conteiner with light
	using MIt = std::map<std::size_t, std::shared_ptr<Model>>::const_iterator; // Iterator on conteiner with light
	
	void SetBackGround(glm::vec4 col);
	const glm::vec4 GetBackGround() const;
	void SetBackGround(std::shared_ptr<Node> box);

	void AddModel(std::shared_ptr<Model> obj);
	void AddLight(std::shared_ptr<Light> lig);
	void AddShader(std::shared_ptr<Shader> sh);

	const std::shared_ptr<Shader> GetShader(std::string_view name) const;
	std::shared_ptr<Model>		  GetModel(std::string_view  name);
	std::shared_ptr<Light>        GetLight(std::string_view  name);
	/*Return const pointer to std::map conteiner with pairs (NameHash-pointerToLight)*/
	const std::pair<LIt, LIt> GetLights() const;
	/*Return const pointer to std::map conteiner with pairs (NameHash-pointerToModels)*/
	const std::pair<MIt, MIt> GetModels() const;

	void Draw();

	void SetCam(std::shared_ptr<Camera>);
	std::shared_ptr<Camera> GetCam() const;

	static inline std::shared_ptr<Model> DefaultPointLightModel;
	static inline std::shared_ptr<Node> DefaultSkyBox;
	bool SkyBoxSetted = false;

	Scene_Information GetInfo();
	void SetController(std::shared_ptr <Position_Controller>);

	Scene(std::shared_ptr <Position_Controller>, std::function<void()> fun = []() {});
	~Scene();
	//bool LoadModels(const std::string& path);
};


