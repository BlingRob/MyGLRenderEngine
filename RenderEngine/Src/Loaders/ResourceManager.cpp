#include "../../Headers/Loaders/ResourceManager.h"

void ResourceManager::LoadDefaultSkyBox() 
{
    //Data base is opened? Load from it
    if (_pDBContr->IsOpen())
    {
        std::vector<std::shared_ptr<Image>> vecs;
        vecs.reserve(Default_SkyBox_names.size());
        for (const auto& ImgName : Default_SkyBox_names)
        {
            auto [ptr, bytes] = _pDBContr->Load("image", "Textures", "name = '" + ImgName + "'");
            if (bytes)
                vecs.emplace_back(ImageLoader::LoadTexture(ptr.get(), bytes));
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
    SceneLoader lod;
    if (_pDBContr->IsOpen())
    {
        auto [ptr, bytes] = _pDBContr->Load("data", "Models", "name = 'PointLight';");
        
        char* text = static_cast<char*>(ptr.get());

        lod.LoadScene(ptr.get(), bytes, "obj");
    }
    else
    {
        constexpr std::size_t BufSize = 128;
        char buffer[BufSize];
        bool PointLightFound = false;
        const char* FileName = "PointLight.obj";

        //Textures in current dir ?
        snprintf(buffer, BufSize, Default_paths[4], Default_paths[1], FileName);
        if (FileIsExist(buffer))
            PointLightFound = !PointLightFound;
        else
        {
            //Textures in prevent dir ?
            snprintf(buffer, BufSize, Default_paths[4], Default_paths[2], FileName);
            if (FileIsExist(buffer))
                PointLightFound = !PointLightFound;
        }
        if (PointLightFound)
            lod.LoadScene(buffer);
    }

    if (lod.IsLoad())
    {
        Scene::DefaultPointLightModel = lod.GetModel(0);
        Scene::DefaultPointLightModel->SetName("PointModel");
        SceneLoader::InitMeshes(Scene::DefaultPointLightModel->GetRoot());
        if (Scene::DefaultPointLightModel)
            Scene::DefaultPointLightModel->SetShader(ShadersBank::GetShader("PointLight"));
    }
}

ResourceManager::ResourceManager()
{
    _pDBContr = std::make_unique<DBController>("./data.bin");
    LoadDefaultShaders();
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
    const float cubePos[24] =
    {
         -1.0f,   1.0f,  -1.0f,
         1.0f,    1.0f,  -1.0f,
         1.0f,    -1.0f,  -1.0f,
         -1.0f,  -1.0f,  -1.0f,
         -1.0f,  1.0f,   1.0f,
         1.0f,   1.0f,   1.0f,
         1.0f, -1.0f,   1.0f,
         -1.0f,  -1.0f,  1.0f
    };
    std::unique_ptr<Node> curNode = std::make_unique<Node>();
    curNode->SetName("SkyBox");
    //Cube's vertices
    std::vector<std::uint32_t> indices = std::vector<std::uint32_t>
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

    std::shared_ptr<Texture> texture = std::make_shared<Texture>(std::string("SkyBox"), std::string("Unknown"), Texture_Types::Skybox, std::move(imgs));
    std::shared_ptr<Mesh> curMesh = std::make_shared<Mesh>(std::make_unique<Vertexes>(24, cubePos, std::move(indices)), nullptr, std::move(std::vector<std::shared_ptr<Texture>>(1, texture)));
    curMesh->setupMesh();
    curNode->addMesh(curMesh);

    return curNode;
}

void ResourceManager::LoadDefaultShaders() 
{
    std::shared_ptr<Shader> shader;

    shader = std::make_shared<Shader>(LoadTextFile("../Shaders/Base.vs"), LoadTextFile("../Shaders/Base.frag"));
    shader->SetName("Default");
    ShadersBank::AddShader(shader);

    shader = std::make_shared<Shader>(LoadTextFile("../Shaders/Light.vs"), LoadTextFile("../Shaders/Light.frag"));
    shader->SetName("PointLight");
    ShadersBank::AddShader(shader);

    shader = std::make_shared<Shader>(LoadTextFile("../Shaders/SkyBox.vs"), LoadTextFile("../Shaders/SkyBox.frag"));
    shader->SetName("SkyBox");
    ShadersBank::AddShader(shader);

    shader = std::make_shared<Shader>(LoadTextFile("../Shaders/PointDepthMap.vs"), LoadTextFile("../Shaders/PointDepthMap.frag"), LoadTextFile("../Shaders/PointDepthMap.gs"));
    shader->SetName("PointShadow");
    ShadersBank::AddShader(shader);

    shader = std::make_shared<Shader>(LoadTextFile("../Shaders/DirectDepthMap.vs"), LoadTextFile("../Shaders/DirectDepthMap.frag"));
    shader->SetName("DirectShadow");
    ShadersBank::AddShader(shader);

    shader = std::make_shared<Shader>(LoadTextFile("../Shaders/PostEffects.vs"), LoadTextFile("../Shaders/PostEffects.frag"));
    shader->SetName("PostEffect");
    ShadersBank::AddShader(shader);
}

ResourceManager::~ResourceManager()
{
    Scene::DefaultSkyBox.reset();
    Scene::DefaultPointLightModel.reset();
    ShadersBank::Clear();
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