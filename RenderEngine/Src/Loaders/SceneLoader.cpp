#include "../../Headers/Loaders/SceneLoader.h"

SceneLoader::SceneLoader()
{
    loaded = false;
}

bool SceneLoader::LoadScene(std::string_view path)
{
    // read file via ASSIMP
    _pImporter = std::make_shared<Assimp::Importer>();
    _pImporter->ReadFile(path.data(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    _pScene = _pImporter->GetScene();
    if (!_pScene || _pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !_pScene->mRootNode) // if is Not Zero
    {
        error = "ERROR::ASSIMP ";
        loaded = false;
        return false;
    }

    loaded = true;

    return loaded;
}

bool SceneLoader::LoadScene(void* memAdr, size_t bytes,const char* FileExtension)
{
    // read file via ASSIMP from memory
    _pImporter = std::make_shared<Assimp::Importer>();
    _pImporter->ReadFileFromMemory(memAdr, bytes, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace, FileExtension);
    // check for errors
    _pScene = _pImporter->GetScene();
    if (!_pScene || _pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) // if is Not Zero
    {
        error = _pImporter->GetErrorString();
        loaded = false;
        return false;
    }
    loaded = true;

    return loaded;
}

std::unique_ptr<Light> SceneLoader::GetLight(std::uint32_t index)
{
    if (!HasLight() || index > NumLights())
        return std::unique_ptr<Light>();
    aiLight* OriginLight = _pScene->mLights[index];

    glm::mat4 transform;
    GetTransform(transform, _pScene->mRootNode->FindNode(OriginLight->mName)->mParent->mTransformation);
    return processLight(OriginLight, transform);
}

std::unique_ptr<Camera> SceneLoader::GetCamera(std::uint32_t index)
{
    if (!HasCamera() || index > NumCameras())
        return nullptr;

    aiCamera* OriginCam = _pScene->mCameras[index];

    glm::mat4 transform;
    GetTransform(transform, _pScene->mRootNode->FindNode(OriginCam->mName)->mParent->mTransformation);
        
    return processCamera(OriginCam, transform);
}

std::unique_ptr<Model> SceneLoader::GetModel(uint32_t index)
{
    if (index > NumModels())
        return nullptr;

    return processModel(_pScene->mRootNode->mChildren[index]);
}

std::unique_ptr<Model>  SceneLoader::GetModel(std::string_view name) 
{
    if (NumModels() < 1)
        return nullptr;

    return processModel(_pScene->mRootNode->FindNode(name.data()));
}
std::unique_ptr<Light>  SceneLoader::GetLight(std::string_view name) 
{
    if (!HasLight())
        return std::unique_ptr<Light>();
    
    aiNode* nativeNode = _pScene->mRootNode->FindNode(name.data());

    if(!nativeNode)
        return std::unique_ptr<Light>();

    glm::mat4 transform;
    GetTransform(transform, nativeNode->mParent->mTransformation);

    aiLight* OriginLight;

    for (uint32_t i = 0; i < NumLights(); ++i)
        if (!std::strcmp(name.data(), _pScene->mLights[i]->mName.C_Str()))
        {
            OriginLight = _pScene->mLights[i];
            break;
        }
    return processLight(OriginLight, transform);
}
std::unique_ptr<Camera> SceneLoader::GetCamera(std::string_view name) 
{
    if (!HasCamera())
        return nullptr;

    aiNode* nativeNode = _pScene->mRootNode->FindNode(name.data());

    glm::mat4 transform;
    GetTransform(transform, nativeNode->mParent->mTransformation);

    aiCamera* OriginCamera;

    for (uint32_t i = 0; i < NumCameras(); ++i)
        if (!std::strcmp(name.data(), _pScene->mCameras[i]->mName.C_Str()))
        {
            OriginCamera = _pScene->mCameras[i];
            break;
        }
    return processCamera(OriginCamera, transform);
}

std::unique_ptr<Model> SceneLoader::processModel(aiNode* root)
{
    std::unique_ptr<Model> model = std::make_unique<Model>([_pImporter = _pImporter]() {});
    model->SetName(root->mName.C_Str());
    model->SetRoot(processNode(root));
    return model;
}

std::unique_ptr<Light>  SceneLoader::processLight(aiLight* OriginLight, const glm::mat4& transform)
{
    glm::vec3 amb = glm::vec3(OriginLight->mColorAmbient.r,
        OriginLight->mColorAmbient.g,
        OriginLight->mColorAmbient.b),
        dif = glm::vec3(OriginLight->mColorDiffuse.r,
            OriginLight->mColorDiffuse.g,
            OriginLight->mColorDiffuse.b),
        spec = glm::vec3(OriginLight->mColorSpecular.r,
            OriginLight->mColorSpecular.g,
            OriginLight->mColorSpecular.b),
        pos = glm::vec3(transform[3][0], transform[3][0], transform[3][0]),
        dir = glm::vec3(OriginLight->mDirection.x,
            OriginLight->mDirection.y,
            OriginLight->mDirection.z),
        clq = glm::vec3(OriginLight->mAttenuationConstant,
            OriginLight->mAttenuationLinear,
            OriginLight->mAttenuationQuadratic);

    std::unique_ptr<Light> light;
    switch (OriginLight->mType)
    {
    case aiLightSource_POINT:
        light = std::make_unique<Light>(PointLight(amb, dif, spec, clq, pos));
        break;
    case aiLightSource_SPOT:
        light = std::make_unique<Light>(SpotLight(amb, dif, spec, clq, pos, dir,
            OriginLight->mAngleOuterCone,
            OriginLight->mAngleInnerCone));
        break;
    default:
    case aiLightSource_DIRECTIONAL:
        light = std::make_unique<Light>(DirectionalLight(amb, dif, spec, clq, glm::mat3(transform) * dir));
        break;
    }
    light->SetName(OriginLight->mName.C_Str());
    return light;
}

std::unique_ptr<Camera> SceneLoader::processCamera(aiCamera* OriginCam, const glm::mat4& transform)
{
    if (!OriginCam)
        return nullptr;
    std::unique_ptr<Camera> cam = std::make_unique<Camera>();

    cam->Position = transform *
                 glm::vec4(cam->Position, 1.0f);
    cam->Front = glm::mat3(transform) *
                 glm::vec3(OriginCam->mLookAt.x,
                  OriginCam->mLookAt.y,
                  OriginCam->mLookAt.z);
    cam->WorldUp = glm::vec3(OriginCam->mUp.x,
                  OriginCam->mUp.y,
                  OriginCam->mUp.z);// * glm::mat3(transform);
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
    cam->SetName(OriginCam->mName.C_Str());

    return cam;
}

std::shared_ptr<Node> SceneLoader::processNode(aiNode* node)
{
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
        auto IterMesh = GlobalMeshes.find(std::hash<std::string>{}(std::string(_pScene->mMeshes[node->mMeshes[i]]->mName.C_Str())));
        if(IterMesh != GlobalMeshes.end())
            curNode->addMesh(IterMesh->second.lock());
        else
            curNode->addMesh(processMesh(_pScene->mMeshes[node->mMeshes[i]]));
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
    aiMaterial* mat = _pScene->mMaterials[mesh->mMaterialIndex];

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
    CurMesh->material = processMaterial(mat, mesh->mMaterialIndex);

    // Loading texture's maps
    std::vector<std::shared_ptr<Texture>> texes;
    // 1. diffuse maps
    texes = processTexture(mat, aiTextureType_DIFFUSE, Texture_Types::Diffuse);
    //CurMesh->textures.emplace_back(CurMesh->textures.end(), texes.begin(), texes.end());
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 2. specular maps
    texes = processTexture(mat, aiTextureType_SPECULAR, Texture_Types::Specular);
    //CurMesh->textures.emplace_back(std::move(texes));
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 3. normal maps
    texes = processTexture(mat, aiTextureType_NORMALS, Texture_Types::Normal);//aiTextureType_HEIGHT
    //CurMesh->textures.emplace_back(std::move(texes));
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 4. height maps
    texes = processTexture(mat, aiTextureType_AMBIENT, Texture_Types::Height);
    //CurMesh->textures.emplace_back(std::move(texes));
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 5. emissive maps
    texes = processTexture(mat, aiTextureType_EMISSIVE, Texture_Types::Emissive);
    //CurMesh->textures.emplace_back(std::move(texes));
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    texes = processTexture(mat, aiTextureType_UNKNOWN, Texture_Types::Metallic_roughness);
    //CurMesh->textures.emplace_back(std::move(texes));
    //CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    /*// 6. metallic maps
    texes = loadTexture(mat, aiTextureType_METALNESS, "texture_metallic");
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 7. roughness maps
    texes = loadTexture(mat, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness");
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());*/
    // 8. ambient_occlusion
    texes = processTexture(mat, aiTextureType_AMBIENT_OCCLUSION, Texture_Types::Ambient_occlusion);
    //CurMesh->textures.emplace_back(std::move(texes));
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());

    // return a mesh object created from the extracted mesh data
    return CurMesh;
}

std::vector<std::shared_ptr<Texture>> SceneLoader::processTexture(aiMaterial* mat, aiTextureType type, Texture_Types typeName)
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

        if (_pScene->HasTextures())// textures are embedded in scene file
        {
             //assimp docs, if tex embedded: path's string has format *TextureIndex
             tex = _pScene->GetEmbeddedTexture(Path.c_str());
             if (!tex->mHeight)
                 texture->imgs.push_back(ImageLoader::LoadTexture(tex->pcData, tex->mWidth));
             else
                 texture->imgs.push_back(std::make_shared<Image>(tex->mWidth, tex->mHeight, 4, (unsigned char*)tex->pcData, [](void* ptr) {}));
        }
        else // textures place in external files 
           texture->imgs.push_back(ImageLoader::LoadTexture(Path.c_str()));        

         texture->name = std::move(Name);
         texture->path = std::move(Path);
         texture->type = typeName;

         textures.emplace_back(std::move(texture));
         GlobalTextures[hash] = textures.back();// store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.    
    }
    return textures;
}

Material SceneLoader::processMaterial(aiMaterial* mat, uint16_t indx)
{
    Material MeshMaterial;

    MeshMaterial.id = indx;
    mat->Get(AI_MATKEY_COLOR_AMBIENT, MeshMaterial.ambient);
    mat->Get(AI_MATKEY_COLOR_DIFFUSE, MeshMaterial.diffuse);
    mat->Get(AI_MATKEY_COLOR_SPECULAR, MeshMaterial.specular);
    mat->Get(AI_MATKEY_SHININESS, MeshMaterial.shininess);

    return MeshMaterial;
}

SceneLoader::~SceneLoader()
{
}

std::unique_ptr<Scene> SceneLoader::GetScene()
{
    if (!IsLoad())
        return nullptr;

    std::unique_ptr<Scene> scen = std::make_unique<Scene>(nullptr);
    if (HasCamera())
        scen->SetCam(GetCamera(0));

    std::shared_ptr <Light> light;
    if (!HasLight())
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
        for (int32_t i = 0; i < NumLights(); ++i)
        {
            light = std::shared_ptr<Light>(GetLight(i));
            scen->AddLight(light);
            if (light->GetType() == LightTypes::Point && Scene::DefaultPointLightModel)
            {
                std::shared_ptr<Model> ModelPointLight = std::make_shared<Model>(*Scene::DefaultPointLightModel);
                ModelPointLight->GetRoot()->tr = light->tr.Get();
                ModelPointLight->SetName(light->GetName());
                scen->AddModel(ModelPointLight);
            }
        }

    std::shared_ptr<Shader> sh = scen->GetShader("Default");
    //#pragma omp parallel for shared(sh, scen)
    #pragma omp parallel for
    for (int32_t i = 0; i < NumModels(); ++i)
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
        InitMeshes(it->second->GetRoot());
    
    return scen;
}

void SceneLoader::GetTransform(glm::mat4& whereTo, const aiMatrix4x4& FromWhere)
{
    whereTo[0] = glm::vec4(FromWhere[0][0], FromWhere[1][0], FromWhere[2][0], FromWhere[3][0]);
    whereTo[1] = glm::vec4(FromWhere[0][1], FromWhere[1][1], FromWhere[2][1], FromWhere[3][1]);
    whereTo[2] = glm::vec4(FromWhere[0][2], FromWhere[1][2], FromWhere[2][2], FromWhere[3][2]);
    whereTo[3] = glm::vec4(FromWhere[0][3], FromWhere[1][3], FromWhere[2][3], FromWhere[3][3]);
}

void SceneLoader::InitMeshes(std::shared_ptr<Node> node)
{
    std::pair<Node::MIt, Node::MIt> meshes = node->GetMeshes();
    for (Node::MIt it = meshes.first; it != meshes.second; ++it)
        it->get()->setupMesh();

    std::pair<Node::NIt, Node::NIt> chld = node->GetChildren();
    for (Node::NIt it = chld.first; it != chld.second; ++it)
        InitMeshes(*it);
}