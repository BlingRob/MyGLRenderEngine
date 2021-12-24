#pragma once
#include "../Scene.h"
//scence's loader
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>
//Texture loader
#include "../Loaders/ImageLoader.h"
#include "SDL.h"
//OpenMp
#include <omp.h>
#include "../../Headers/ParallelStructures.h"


class SceneLoader
{
public:
	std::unique_ptr<Scene>  GetScene();
	std::unique_ptr<Model>  GetModel(uint32_t);
	std::unique_ptr<Light>  GetLight(uint32_t);
	std::unique_ptr<Camera> GetCamera(uint32_t);
	std::unique_ptr<Model>  GetModel(std::string_view);
	std::unique_ptr<Light>  GetLight(std::string_view);
	std::unique_ptr<Camera> GetCamera(std::string_view);

	inline bool IsLoad()
	{
		return loaded;
	}
	inline bool HasCamera()
	{
		return IsLoad() ? _pScene->HasCameras() : false;
	}
	inline bool HasLight()
	{
		return IsLoad() ? _pScene->HasLights() : false;
	}
	inline uint32_t NumLights()
	{
		return IsLoad() ? _pScene->mNumLights : 0;
	}
	inline uint32_t NumCameras()
	{
		return IsLoad() ? _pScene->mNumCameras : 0;
	}
	inline uint32_t NumModels()
	{
		return IsLoad() ? _pScene->mRootNode->mNumChildren : 0;
	}

	bool LoadScene(std::string_view path);
	bool LoadScene(void* memAdr, size_t bytes,const char* FileExtension);

	SceneLoader();
	~SceneLoader();
private:
	const aiScene* _pScene;
	std::shared_ptr<Assimp::Importer> _pImporter;

	bool loaded;
	std::string error;

	ParallelMap<std::size_t, std::weak_ptr<Mesh>> GlobalMeshes;
	ParallelMap<std::size_t, std::weak_ptr<Texture>> GlobalTextures;

	std::unique_ptr<Model>					processModel(aiNode* root);
	std::shared_ptr<Node>					processNode(aiNode* node);
	std::shared_ptr<Mesh>					processMesh(aiMesh* mesh);
	std::unique_ptr<Light>					processLight(aiLight*, const glm::mat4&);
	std::unique_ptr<Camera>					processCamera(aiCamera*, const glm::mat4&);
	std::vector< std::shared_ptr<Texture>>  processTexture(aiMaterial* mat, aiTextureType type, Texture_Types MyType);
	Material								processMaterial(aiMaterial* mat, uint16_t indx);

	void GetTransform(glm::mat4& whereTo, const aiMatrix4x4& FromWhere);
	void InitMeshes(std::shared_ptr<Node>);
};


