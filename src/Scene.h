#pragma once
#include "Headers.h"
#include "Light.h"
#include "Camera.h"
#include "Node.h"
#include "Model.h"
#include "Transformation.h"
#include "FrameBuffer.h"

struct Scene_Information
{
	std::size_t Amount_models,
				Amount_lights,
				Amount_shaders;
};

class Scene
{
	std::unique_ptr<Camera> camera;
	std::map<std::size_t, std::shared_ptr<Model>> Models;
	std::map<std::size_t, std::shared_ptr<Light>> Lights;
	std::map<std::size_t, std::shared_ptr<Shader>> Shaders;

	glm::vec4 BackGroundColour;
	std::shared_ptr<Node> SkyBox;
	
	GLuint ShadowMapSize = 2048;
	std::unique_ptr<ShadowMapBuffer> ShadowMap;
	Matrices LightMat;//Translation-projection Shadow matrices
	glm::mat4 scale_bias_matrix; 
	void ShadowDraw();

	public:
	
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
	const std::map<std::size_t, std::shared_ptr<Light>>* GetLights() const;

	void Draw();

	Matrices matrs;

	void SetCam(std::unique_ptr<Camera>);
	Camera* GetCam() const;

	static inline std::shared_ptr<Model> DefaultPointLightModel;
	static inline std::shared_ptr<Node> DefaultSkyBox;
	bool SkyBoxSetted = false;

	Scene_Information GetInfo();

	Scene();
	//bool LoadModels(const std::string& path);
};


