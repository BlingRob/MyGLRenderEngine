#include "../../Headers/Loaders/SceneLoader.h"

SceneLoader::SceneLoader()
{
    loaded = false;
    IndexModel = -1;
    IndexLight = -1;
};
bool SceneLoader::LoadScene(std::string_view path)
{
    // read file via ASSIMP
    importer = std::make_shared<Assimp::Importer>();
    importer->ReadFile(path.data(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    //_mscene = std::unique_ptr<aiScene>(importer.GetOrphanedScene());
    // check for errors
    _mscene = importer->GetScene();
    if (!_mscene || _mscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !_mscene->mRootNode) // if is Not Zero
    {
        error = "ERROR::ASSIMP ";
        loaded = false;
        return false;
    }
    // retrieve the directory path of the filepath
    #if defined(__WIN32__)
        directory = path.substr(0, path.find_last_of('\\'));
    #else
        directory = path.substr(0, path.find_last_of('/'));
    #endif
    IndexModel = _mscene->mRootNode->mNumChildren - 1;
    IndexLight = _mscene->mNumLights - 1;

    loaded = true;

    return true;
}

void SceneLoader::Destroy()
{
}

SceneLoader::SceneLoader(std::string_view path)
{
    loaded = LoadScene(path);
}

bool SceneLoader::Is_Load()
{
    return loaded;
}

bool SceneLoader::Has_Camera()
{
    return _mscene->HasCameras();
}
bool SceneLoader::Has_Light()
{
    return _mscene->HasLights();
}

std::unique_ptr<Light> SceneLoader::GetLight()
{
    if (!Has_Light() || IndexLight == -1)
        return std::unique_ptr<Light>(nullptr);
    aiNode* LightNode = _mscene->mRootNode->FindNode(_mscene->mLights[IndexLight]->mName);
    if(LightNode == nullptr)
        return std::unique_ptr<Light>(nullptr);

    glm::vec3 amb =  glm::vec3(_mscene->mLights[IndexLight]->mColorAmbient.r,
                                _mscene->mLights[IndexLight]->mColorAmbient.g,
                                _mscene->mLights[IndexLight]->mColorAmbient.b),
              dif =  glm::vec3(_mscene->mLights[IndexLight]->mColorDiffuse.r,
                              _mscene->mLights[IndexLight]->mColorDiffuse.g,
                              _mscene->mLights[IndexLight]->mColorDiffuse.b),
              spec = glm::vec3(_mscene->mLights[IndexLight]->mColorSpecular.r,
                              _mscene->mLights[IndexLight]->mColorSpecular.g,
                              _mscene->mLights[IndexLight]->mColorSpecular.b),
              pos =  glm::vec3(LightNode->mParent->mTransformation[0][3],
                               LightNode->mParent->mTransformation[1][3],
                               LightNode->mParent->mTransformation[2][3]),
              dir = glm::vec3(_mscene->mLights[IndexLight]->mDirection.x,
                              _mscene->mLights[IndexLight]->mDirection.y,
                              _mscene->mLights[IndexLight]->mDirection.z),
              clq = glm::vec3(_mscene->mLights[IndexLight]->mAttenuationConstant,
                              _mscene->mLights[IndexLight]->mAttenuationLinear,
                              _mscene->mLights[IndexLight]->mAttenuationQuadratic);
    glm::mat4 transform;
    GetTransform(transform, LightNode->mParent->mTransformation);

    std::unique_ptr<Light> light;
    switch(_mscene->mLights[IndexLight]->mType)
    {
        case aiLightSource_POINT:
            light = std::make_unique<Light>(PointLight(amb, dif, spec, clq, pos));
        break;
        case aiLightSource_SPOT:
            light = std::make_unique<Light>(SpotLight(amb, dif, spec, clq, pos, dir,
                _mscene->mLights[IndexLight]->mAngleOuterCone,
                _mscene->mLights[IndexLight]->mAngleInnerCone));
        break;
        default:
        case aiLightSource_DIRECTIONAL:
            light = std::make_unique<Light>(DirectionalLight(amb, dif, spec, clq, glm::mat3(transform) * dir));
        break;
    }
    light->SetName(_mscene->mLights[IndexLight]->mName.C_Str());
    --IndexLight;
    return std::move(light);
}
std::unique_ptr<Camera> SceneLoader::GetCamera()
{
    if (!Has_Camera())
        return nullptr;

    glm::mat4 transform;

    aiNode* CameraNode = _mscene->mRootNode->FindNode(_mscene->mCameras[0]->mName);

    if (CameraNode->mParent != nullptr)
        GetTransform(transform, CameraNode->mParent->mTransformation);

    std::unique_ptr<Camera> cam = std::make_unique<Camera>();

    cam->Position = transform * glm::vec4(cam->Position, 1.0f);
    cam->Front = glm::mat3(transform) * glm::vec3(_mscene->mCameras[0]->mLookAt.x,
                                            _mscene->mCameras[0]->mLookAt.y,
                                            _mscene->mCameras[0]->mLookAt.z);
    cam->WorldUp = glm::vec3(_mscene->mCameras[0]->mUp.x,
                            _mscene->mCameras[0]->mUp.y,
                            _mscene->mCameras[0]->mUp.z);// * glm::mat3(transform);
    /*
    
    if (CameraNode->mParent != nullptr)
    {
        scene->mCameras[0]->mPosition.x = CameraNode->mParent->mTransformation[0][3];
        scene->mCameras[0]->mPosition.y = CameraNode->mParent->mTransformation[1][3];
        scene->mCameras[0]->mPosition.z = CameraNode->mParent->mTransformation[2][3];
    }

    std::unique_ptr<Camera> cam = std::make_unique<Camera>
        (
            glm::vec3(scene->mCameras[0]->mPosition.x, 
                      scene->mCameras[0]->mPosition.y, 
                      scene->mCameras[0]->mPosition.z),
            glm::vec3(scene->mCameras[0]->mLookAt.x,
                      scene->mCameras[0]->mLookAt.y,
                      scene->mCameras[0]->mLookAt.z),
            glm::vec3(scene->mCameras[0]->mUp.x,
                      scene->mCameras[0]->mUp.y,
                      scene->mCameras[0]->mUp.z)
        );*/
    cam->SetName(_mscene->mCameras[0]->mName.C_Str());

    return std::move(cam);
}

std::unique_ptr<Model> SceneLoader::GetModel(uint32_t Indx)
{
    if (!_mscene || !_mscene->mRootNode || Indx == -1)
        return nullptr;
    
    std::unique_ptr<Model> model = std::make_unique<Model>();
    model->SetName(_mscene->mRootNode->mChildren[Indx]->mName.C_Str());
    model->SetRoot(processNode(_mscene->mRootNode->mChildren[Indx]));
    return std::move(model);
}

std::shared_ptr<Node> SceneLoader::processNode(aiNode* node)
{
    const uint16_t MatSize = 4;
    // process each mesh located at the current node
    std::shared_ptr<Node> curNode = std::make_shared<Node>(),tmp;
    curNode->SetName(node->mName.C_Str());
    std::shared_ptr<glm::mat4> ModelMatr = std::make_shared<glm::mat4>(1.0f),TmpMat;
    GetTransform(*ModelMatr, node->mTransformation);
    curNode->tr.Set(ModelMatr);
    for (std::size_t i = 0; i < node->mNumMeshes; ++i)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        auto IterMesh = GlobalMeshes.find(std::hash<std::string>{}(std::string(_mscene->mMeshes[node->mMeshes[i]]->mName.C_Str())));
        if(IterMesh != GlobalMeshes.end())
            curNode->addMesh(IterMesh->second.lock());
        else
            curNode->addMesh(processMesh(_mscene->mMeshes[node->mMeshes[i]]));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (std::size_t i = 0; i < node->mNumChildren; ++i)
    {
        tmp = processNode(node->mChildren[i]);
        TmpMat = tmp->tr.Get();
        *TmpMat = *ModelMatr * (*TmpMat);
        tmp->tr.Set(TmpMat);
        curNode->addChild(tmp);
    }
    return curNode;
}

std::shared_ptr <Mesh> SceneLoader::processMesh(aiMesh* mesh)
{
    std::shared_ptr<Mesh> CurMesh = std::make_shared<Mesh>();
    CurMesh->SetName(mesh->mName.C_Str());
    // process materials
    aiMaterial* mat = _mscene->mMaterials[mesh->mMaterialIndex];

    //Create additional thread for load geometry and material of mesh
   // walk through each of the mesh's vertices

    if (mesh->HasPositions())
    {
        CurMesh->vertices._msizes[Vertexes::PointTypes::positions] = mesh->mNumVertices * Mesh::CardCoordsPerPoint;
        CurMesh->vertices.Positions = mesh->mVertices;
    }
    if (mesh->HasNormals()) 
    {
        CurMesh->vertices._msizes[Vertexes::PointTypes::normals] = mesh->mNumVertices * Mesh::CardCoordsPerPoint;
        CurMesh->vertices.Normals = mesh->mNormals;
    }
    if (mesh->HasTangentsAndBitangents()) 
    {
        CurMesh->vertices._msizes[Vertexes::PointTypes::tangent] = mesh->mNumVertices * Mesh::CardCoordsPerPoint;
        CurMesh->vertices.Tangents = mesh->mTangents;

        CurMesh->vertices._msizes[Vertexes::PointTypes::bitangent] = mesh->mNumVertices * Mesh::CardCoordsPerPoint;
        CurMesh->vertices.Bitangents = mesh->mBitangents;
    }
    if (mesh->HasTextureCoords(0)) 
    {
        CurMesh->vertices._msizes[Vertexes::PointTypes::texture] = mesh->mNumVertices * Mesh::CardCoordsPerPoint;
        CurMesh->vertices.TexCoords = mesh->mTextureCoords[0];
    }
   
    CurMesh->indices.reserve(mesh->mNumFaces * mesh->mFaces[0].mNumIndices);//Indices = 3 * card of face's number
    for (std::size_t i = 0; i < mesh->mNumFaces; ++i)
    {
           // retrieve all indices of the face and store them in the indices vector
        for (std::size_t j = 0; j < mesh->mFaces[i].mNumIndices; j++)
            CurMesh->indices.push_back(mesh->mFaces[i].mIndices[j]);
    }

    //load materials
    CurMesh->material = loadMaterial(mat, mesh->mMaterialIndex);

    // Loading texture's maps
    std::vector< std::shared_ptr<Texture>> texes;
    // 1. diffuse maps
    texes = loadTexture(mat, aiTextureType_DIFFUSE, Texture_Types::Diffuse);
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 2. specular maps
    texes = loadTexture(mat, aiTextureType_SPECULAR, Texture_Types::Specular);
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 3. normal maps
    texes = loadTexture(mat, aiTextureType_NORMALS, Texture_Types::Normal);//aiTextureType_HEIGHT
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 4. height maps
    texes = loadTexture(mat, aiTextureType_AMBIENT, Texture_Types::Height);
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 5. emissive maps
    texes = loadTexture(mat, aiTextureType_EMISSIVE, Texture_Types::Emissive);
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    texes = loadTexture(mat, aiTextureType_UNKNOWN, Texture_Types::Metallic_roughness);
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    /*// 6. metallic maps
    texes = loadTexture(mat, aiTextureType_METALNESS, "texture_metallic");
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 7. roughness maps
    texes = loadTexture(mat, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness");
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());*/
    // 8. ambient_occlusion
    texes = loadTexture(mat, aiTextureType_AMBIENT_OCCLUSION, Texture_Types::Ambient_occlusion);
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());

    // return a mesh object created from the extracted mesh data
    return CurMesh;
}

std::vector<std::shared_ptr<Texture>> SceneLoader::loadTexture(aiMaterial* mat, aiTextureType type, Texture_Types typeName)
{
    std::vector<std::shared_ptr<Texture>> textures;
    aiString path, name;
    std::string Path,Name;
    void* TexturePtr;
    const aiTexture* tex;
    uint32_t Width, Height;

    textures.reserve(mat->GetTextureCount(type));
    for (std::size_t i = 0; i < mat->GetTextureCount(type); ++i)
    {
        Width = 0;
        Height = 0;
        mat->Get(AI_MATKEY_NAME, name);
        mat->Get(AI_MATKEY_TEXTURE(type, static_cast<unsigned int>(0)), path);

        Path = std::string(path.C_Str());
        Name = std::string(name.C_Str());

        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        // if texture hasn't been loaded already, load it
        std::size_t hash = std::hash<std::string>{}(Name + Path);
        auto node = GlobalTextures.find(hash);
        if (node != GlobalTextures.end())
        {
            if (std::find(textures.begin(), textures.end(), node->second.lock()) == textures.end())
            {
                textures.push_back(node->second.lock());
                continue;
            }
        }
        std::shared_ptr <Texture> texture(std::make_shared<Texture>());

        if (_mscene->HasTextures())// textures are embedded in scene file
        {
             //assimp docs, if tex embedded: path's string has format *TextureIndex
             tex = _mscene->GetEmbeddedTexture(Path.c_str());
             if (!tex->mHeight)
                 texture->imgs.push_back(ImageLoader::LoadTexture(tex->pcData, tex->mWidth));
             else
                 texture->imgs.push_back(std::make_shared<Image>(tex->mWidth, tex->mHeight, 4, (unsigned char*)tex->pcData, [](void* ptr) {}));
        }
        else // textures place in external files 
           //str = this->directory + '\\' + path.C_Str();
           texture->imgs.push_back(ImageLoader::LoadTexture(Path.c_str()));        

         texture->name = std::move(Name);
         texture->path = std::move(Path);
         texture->type = typeName;

         textures.push_back(texture);
         GlobalTextures[hash] = texture;// store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.    
    }
    return std::move(textures);
}

Material SceneLoader::loadMaterial(aiMaterial* mat, uint16_t indx)
{
    Material MeshMaterial;

    MeshMaterial.id = indx;
    //mat->Get(AI_MATKEY_COLOR_AMBIENT, MeshMaterial.ambient);
    mat->Get(AI_MATKEY_COLOR_AMBIENT, MeshMaterial.ambient);
    mat->Get(AI_MATKEY_COLOR_DIFFUSE, MeshMaterial.diffuse);
    mat->Get(AI_MATKEY_COLOR_SPECULAR, MeshMaterial.specular);
    mat->Get(AI_MATKEY_SHININESS, MeshMaterial.shininess);

    return std::move(MeshMaterial);
}


std::unique_ptr<Node> SceneLoader::LoadSkyBox(std::vector<std::string_view> paths)
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
    texture->imgs = ImageLoader::LoadTexture(paths);
    texture->type = Texture_Types::Skybox;
    texture->createGLTex();
    texture->name = "SkyBox";
    texture->path = paths[0].data();

    curMesh->textures.push_back(texture);

    curMesh->setupMesh();
    curNode->addMesh(curMesh);

    return std::move(curNode);
}


SceneLoader::~SceneLoader()
{
}

std::unique_ptr<Scene> SceneLoader::GetScene(std::string_view path)
{
    LoadScene(path);

    if (!Is_Load())
        throw(std::string("Problem with scene loading!"));
    std::shared_ptr<Assimp::Importer> CopyImporter = importer;
    std::unique_ptr<Scene> scen = std::make_unique<Scene>(nullptr, [CopyImporter]() {CopyImporter->FreeScene(); });
    if (Has_Camera())
        scen->SetCam(GetCamera());


    std::shared_ptr <Light> light;
    if (!Has_Light())
    {
        //default light
        
        light = std::make_shared<Light>(DirectionalLight(
                             glm::vec3(1.0f, 1.0f, 1.0f),
                             glm::vec3(0.8f, 0.8f, 0.8f),
                             glm::vec3(1.0f, 1.0f, 1.0f),
                             glm::vec3(1.0f, 0.0f, 0.0f),
                             glm::vec3(0.0f, 0.0f, -1.0f)));
        light->SetName("Default light");
        scen->AddLight(light);
    }
    else
        while ((light = std::shared_ptr<Light>(GetLight().release())))
        {
            scen->AddLight(light);
            if (light->GetType() == LightTypes::Point && Scene::DefaultPointLightModel)
            {
                std::shared_ptr<Model> ModelPointLight(std::make_shared<Model>(*Scene::DefaultPointLightModel.get()));
                ModelPointLight->GetRoot()->tr = light->tr.Get();
                ModelPointLight->SetName(light->GetName());
                //Scene::DefaultPointLightModel->GetRoot()->tr.Set(light->tr.Get());
                scen->AddModel(ModelPointLight);
            }
        }

    std::shared_ptr<Shader> sh = scen->GetShader("Default");
    #pragma omp parallel for shared(sh, scen)
    for (int32_t i = 0; i <= IndexModel; ++i)
    {
        std::unique_ptr<Model> mod = GetModel(i);
        if (mod)
        {
            mod->SetShader(sh);
            #pragma omp critical
            {
                scen->AddModel(std::shared_ptr<Model>(mod.release())); 
            }
        }
    }

    std::pair<Scene::MIt, Scene::MIt> models = scen->GetModels();
    for (Scene::MIt it = models.first; it != models.second; ++it)
        CreateGLObjects(it->second->GetRoot());
    
    return std::move(scen);
}

void SceneLoader::GetTransform(glm::mat4& whereTo, const aiMatrix4x4& FromWhere)
{
    whereTo[0] = glm::vec4(FromWhere[0][0], FromWhere[1][0], FromWhere[2][0], FromWhere[3][0]);
    whereTo[1] = glm::vec4(FromWhere[0][1], FromWhere[1][1], FromWhere[2][1], FromWhere[3][1]);
    whereTo[2] = glm::vec4(FromWhere[0][2], FromWhere[1][2], FromWhere[2][2], FromWhere[3][2]);
    whereTo[3] = glm::vec4(FromWhere[0][3], FromWhere[1][3], FromWhere[2][3], FromWhere[3][3]);
}


void SceneLoader::CreateGLObjects(std::shared_ptr<Node> node)
{
    std::pair<Node::MIt, Node::MIt> meshes = node->GetMeshes();
    for (Node::MIt it = meshes.first; it != meshes.second; ++it)
    {
        it->get()->setupMesh();
        for (auto& tex : it->get()->textures)
            if(!tex->IsCreated())
                tex->createGLTex();
    }
    std::pair<Node::NIt, Node::NIt> chld = node->GetChildren();
    for (Node::NIt it = chld.first; it != chld.second; ++it)
        CreateGLObjects(*it);
}