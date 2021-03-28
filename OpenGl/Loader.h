#pragma once
#include "Headers.h"
#include "Light.h"
#include "Ñamera.h"
#include "Node.h"
#include "Model.h"

class Loader
{
	std::unique_ptr<aiScene> scene;
	std::string directory;

	bool loaded;
	std::string error;
	int64_t IndexModel;
	int64_t IndexLight;

	std::shared_ptr<Node> processNode(aiNode* node);
	std::shared_ptr<Mesh> processMesh(aiMesh* mesh);
	std::vector< std::shared_ptr<Texture>> loadTexture(aiMaterial* mat, aiTextureType type, std::string typeName);
	Material loadMaterial(aiMaterial* mat, uint16_t indx);
	
public:

	static GLuint TextureFromFile(const void* path, std::size_t width, std::size_t height, bool FromProc);
	std::unique_ptr<Model> GetModel();
	std::unique_ptr<Light> GetLight();
	std::unique_ptr<Camera> GetCamera();

	bool Is_Load();
	bool Has_Camera();
	bool Has_Light();

	bool LoadScene(const std::string& path);

	Loader(const std::string& path);
	Loader();
	void Destroy();
	~Loader();
};

