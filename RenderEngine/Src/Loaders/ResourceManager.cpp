#include "../../Headers/Loaders/ResourceManager.h"

bool FileIsExist(std::string_view filePath)
{
    bool isExist = false;
    std::ifstream fin(filePath.data());

    if (fin.is_open())
        isExist = true;

    fin.close();
    return isExist;
}

void ResourceManager::LoadDefaultSkyBox() 
{
    //Data base is opened? Load from it
    if (_pDBContr->IsOpen())
    {
        std::vector<std::shared_ptr<Image>> vecs;
        vecs.reserve(Default_SkyBox_names.size());
        for (const auto& ImgName : Default_SkyBox_names)
        {
            auto [bytes, ptr] = _pDBContr->Load("image", "Textures", "name = '" + ImgName + "'");
            if (bytes)
                vecs.push_back(ImageLoader::LoadTexture(ptr.get(), bytes));
        }

        Scene::DefaultSkyBox = CreateSkyBox(std::move(vecs));
    }
    else
    {
        constexpr std::size_t BufSize = 128;
        char buffer[BufSize];
        bool SkyBoxFound = false;

        //Textures in current dir ?
        snprintf(buffer, BufSize, Default_paths[0], Default_paths[1], Default_paths[3], (Default_SkyBox_names[0] + extension).c_str());
        if (FileIsExist(buffer))
        {
            snprintf(buffer, BufSize, Default_paths[0], Default_paths[1], Default_paths[3]);
            SkyBoxFound = !SkyBoxFound;
        }
        else
        {
            //Textures in prevent dir ?
            snprintf(buffer, BufSize, Default_paths[0], Default_paths[2], Default_paths[3], (Default_SkyBox_names[0] + extension).c_str());
            if (FileIsExist(buffer))
            {
                SkyBoxFound = !SkyBoxFound;
                snprintf(buffer, BufSize, Default_paths[0], Default_paths[2], Default_paths[3]);
            }
        }
        if (SkyBoxFound)
        {
            std::vector<std::string_view> paths(CubicalTexturesSize);
            for (std::size_t i = 0; i < Default_SkyBox_names.size(); ++i)
            {
                snprintf(buffer, BufSize, (Default_SkyBox_names[i] + extension).c_str());
                paths[i] = buffer;
            }
            Scene::DefaultSkyBox = CreateSkyBox(LoadSkyBoxTextures(std::move(paths)));
        }
    }
}
void ResourceManager::LoadDefaultModelPointLight() 
{
    if (_pDBContr->IsOpen())
    {
        

    }
    else
    {
        constexpr std::size_t BufSize = 128;
        char buffer[BufSize];
        bool PointLightFound = false;

        //Textures in current dir ?
        snprintf(buffer, BufSize, Default_textures_paths[0], Default_textures_paths[1], Default_textures_paths[3], (Default_SkyBox_names[0] + extension).c_str());
        if (FileIsExist(buffer))
        {
            snprintf(buffer, BufSize, Default_textures_paths[0], Default_textures_paths[1], Default_textures_paths[3]);
            SkyBoxFound = !SkyBoxFound;
        }
        else
        {
            //Textures in prevent dir ?
            snprintf(buffer, BufSize, Default_textures_paths[0], Default_textures_paths[2], Default_textures_paths[3], (Default_SkyBox_names[0] + extension).c_str());
            if (FileIsExist(buffer))
            {
                SkyBoxFound = !SkyBoxFound;
                snprintf(buffer, BufSize, Default_textures_paths[0], Default_textures_paths[2], Default_textures_paths[3]);
            }
        }
        if (SkyBoxFound)
        {
            std::vector<std::string_view> paths(CubicalTexturesSize);
            for (std::size_t i = 0; i < Default_SkyBox_names.size(); ++i)
            {
                snprintf(buffer, BufSize, (Default_SkyBox_names[i] + extension).c_str());
                paths[i] = buffer;
            }
            Scene::DefaultSkyBox = CreateSkyBox(LoadSkyBoxTextures(std::move(paths)));
        }
    }
}

ResourceManager::ResourceManager()
{
    _pDBContr = std::make_unique<DBController>("./data.bin");
    LoadDefaultSkyBox();
    LoadDefaultModelPointLight();
}

std::vector<std::shared_ptr<Image>> ResourceManager::LoadSkyBoxTextures(std::vector<std::string_view>&& paths)
{
    std::vector<std::shared_ptr<Image>> imgs;
    imgs.reserve(paths.size());
    for (std::size_t i = 0; i < paths.size(); ++i)
        imgs.emplace_back(ImageLoader::LoadTexture(paths[i].data()));
    return imgs;
}

std::unique_ptr<Node> ResourceManager::CreateSkyBox(std::vector<std::shared_ptr<Image>>&& imgs)
{
    std::unique_ptr<Node> curNode = std::make_unique<Node>();
    curNode->SetName("SkyBox");
    std::shared_ptr<Mesh> curMesh = std::make_shared<Mesh>();
    //Cube's vertices
    curMesh->vertices._msizes[Vertexes::PointTypes::positions] = 24;
    curMesh->vertices.Positions = new aiVector3D[8]
    {
        aiVector3D {-1.0f,   1.0f,  -1.0f},
        aiVector3D {1.0f,    1.0f,  -1.0f},
        aiVector3D {1.0f,    -1.0f,  -1.0f},
        aiVector3D {-1.0f,  -1.0f,  -1.0f},
        aiVector3D {-1.0f,  1.0f,   1.0f},
        aiVector3D {1.0f,   1.0f,   1.0f},
        aiVector3D {1.0f, -1.0f,   1.0f},
        aiVector3D {-1.0f,  -1.0f,  1.0f}
    };

    curMesh->indices = std::vector<GLuint>
        (
            {
                0, 1, 2,
                0, 2, 3,
                2, 1, 5,
                2, 5, 6,
                3, 2, 6,
                3, 6, 7,
                0, 3, 7,
                0, 7, 4,
                1, 0, 4,
                1, 4, 5,
                6, 5, 4,
                6, 4, 7
            }
    );

    std::shared_ptr <Texture> texture = std::make_shared<Texture>();
    texture->imgs = std::move(imgs);// ImageLoader::LoadTexture(paths);
    texture->type = Texture_Types::Skybox;
    texture->createGLTex();
    texture->name = "SkyBox";
    texture->path = "Unknown";

    curMesh->textures.push_back(texture);

    curMesh->setupMesh();
    curNode->addMesh(curMesh);

    return curNode;
}

/*
bool Scene::LoadModels(const std::string& path)
{
    Loader loader(path);
    std::shared_ptr<Model> mod;
    while ((mod = std::shared_ptr<Model>(loader.GetModel().release())))
        AddModel(mod);
    return true;///No finished!
}*/