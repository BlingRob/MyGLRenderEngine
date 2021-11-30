#pragma once
#include "../Scene.h"
//scence's loader
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>
//Texture loader
#include "../Loaders/ImageLoader.h"
//Load OGLfuns
#include "SDL.h"
//OpenMp
#include <omp.h>
#include "../../Headers/ParallelStructures.h"


class SceneLoader
{
public:
	static std::unique_ptr<Node> LoadSkyBox(std::vector<std::string_view> paths);
	std::unique_ptr<Scene> GetScene(std::string_view path);
	std::unique_ptr<Model> GetModel(uint32_t);
	std::unique_ptr<Light> GetLight();
	std::unique_ptr<Camera> GetCamera();

	bool Is_Load();
	bool Has_Camera();
	bool Has_Light();

	bool LoadScene(std::string_view path);

	SceneLoader(std::string_view path);
	SceneLoader();
	void Destroy();
	~SceneLoader();
private:

	const aiScene* _mscene;
	std::shared_ptr <Assimp::Importer> importer;
	std::string directory;

	bool loaded;
	std::string error;
	int32_t IndexModel;
	int32_t IndexLight;

	ParallelMap<std::size_t, std::weak_ptr<Mesh>> GlobalMeshes;
	ParallelMap<std::size_t, std::weak_ptr<Texture>> GlobalTextures;

	std::shared_ptr<Node> processNode(aiNode* node);
	std::shared_ptr<Mesh> processMesh(aiMesh* mesh);
	std::vector< std::shared_ptr<Texture>> loadTexture(aiMaterial* mat, aiTextureType type, Texture_Types MyType);
	Material loadMaterial(aiMaterial* mat, uint16_t indx);

	void GetTransform(glm::mat4& whereTo, const aiMatrix4x4& FromWhere);
	void CreateGLObjects(std::shared_ptr<Node>);
};


