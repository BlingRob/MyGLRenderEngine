#pragma once
#include "SceneLoader.h"
#include "ImageLoader.h"
#include "DBController.h"
#include <memory>
#include <array>

bool FileIsExist(std::string_view filePath);

class ResourceManager
{
public:
	static const std::size_t CubicalTexturesSize = 6;

	ResourceManager();
	
	static std::vector<std::shared_ptr<Image>> LoadSkyBoxTextures(std::vector<std::string_view>&& paths);
	std::unique_ptr<Node> CreateSkyBox(std::vector<std::shared_ptr<Image>>&&);
	bool LoadModel(std::string_view path);

private:
	const std::array<std::string, CubicalTexturesSize> Default_SkyBox_names{"right", "left", "top","bottom", "front", "back"};
	const char* Default_paths[5] = { 
										"%s/Textures/%s",//direction with 
										".",//current direction
										"..",//Prevent direction
										"lightblue/%s.png",//default sky box direction
										"%s/Models/%s"//default models direction
								   };
	const char* extension{ ".png" };

	void LoadDefaultSkyBox();
	void LoadDefaultModelPointLight();

	std::unique_ptr<DBController> _pDBContr;
};