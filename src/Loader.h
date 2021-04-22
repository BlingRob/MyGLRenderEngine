#pragma once
#include "Scene.h"
//scence's loader
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>
//Texture's loader
#include "STB/stb_image.h"
//Load OGLfuns
#include <SDL.h>

class Loader
{
	std::unique_ptr<aiScene> scene;
	std::string directory;

	bool loaded;
	std::string error;
	int32_t IndexModel;
	int32_t IndexLight;

	std::shared_ptr<Node> processNode(aiNode* node);
	std::shared_ptr<Mesh> processMesh(aiMesh* mesh);
	std::vector< std::shared_ptr<Texture>> loadTexture(aiMaterial* mat, aiTextureType type, std::string typeName);
	Material loadMaterial(aiMaterial* mat, uint16_t indx);
	/*path - maybe address in programm, if FromProc is true, then texture loading from current process;
	  if FromProc is false path must be pointer to const char* with internal file path*/
	static GLuint TextureFromFile(const void* path, std::size_t width, std::size_t height, bool FromProc);
	static GLuint CubeTextureFromFile(std::vector<std::string_view> paths);
	
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

	Loader(std::string_view path);
	Loader();
	void Destroy();
	~Loader();
};

