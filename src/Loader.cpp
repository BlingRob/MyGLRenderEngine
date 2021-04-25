#include "Loader.h"

Loader::Loader()
{
    loaded = false;
    IndexModel = -1;
    IndexLight = -1;
};
bool Loader::LoadScene(std::string_view path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    importer.ReadFile(path.data(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    scene = std::unique_ptr<aiScene>(importer.GetOrphanedScene());
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        error = "ERROR::ASSIMP ";
        loaded = false;
        return false;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('\\'));
    IndexModel = scene->mRootNode->mNumChildren - 1;
    IndexLight = scene->mNumLights - 1;

    loaded = true;

    while (scene->mRootNode->mChildren[IndexModel]->mNumMeshes == 0)//Skip camera or lights nodes
        --IndexModel;

    return true;
}

void Loader::Destroy() 
{
    delete scene.release();
    scene.reset(nullptr);
}

Loader::Loader(std::string_view path)
{
    loaded = LoadScene(path);
}

bool Loader::Is_Load() 
{
    return loaded;
}

bool Loader::Has_Camera() 
{
    return scene->HasCameras();
}
bool Loader::Has_Light() 
{
    return scene->HasLights();
}

std::unique_ptr<Light> Loader::GetLight()
{
    if (!Has_Light() || IndexLight == -1)
        return std::unique_ptr<Light>(nullptr);
    aiNode* LightNode = scene->mRootNode->FindNode(scene->mLights[IndexLight]->mName);
    /*if (LightNode != nullptr) 
    {
        scene->mLights[IndexLight]->mPosition.x = LightNode->mParent->mTransformation[0][3];
        scene->mLights[IndexLight]->mPosition.y = LightNode->mParent->mTransformation[1][3];
        scene->mLights[IndexLight]->mPosition.z = LightNode->mParent->mTransformation[2][3];
    }*/
    if(LightNode == nullptr)
        return std::unique_ptr<Light>(nullptr);

    std::unique_ptr<Light> light = std::make_unique<Light>
        (   scene->mLights[IndexLight]->mAttenuationConstant,
            scene->mLights[IndexLight]->mAttenuationLinear,
            scene->mLights[IndexLight]->mAttenuationQuadratic,
            glm::vec3(scene->mLights[IndexLight]->mColorAmbient.r,
                      scene->mLights[IndexLight]->mColorAmbient.g, 
                      scene->mLights[IndexLight]->mColorAmbient.b),
            glm::vec3(scene->mLights[IndexLight]->mColorDiffuse.r,
                      scene->mLights[IndexLight]->mColorDiffuse.g,
                      scene->mLights[IndexLight]->mColorDiffuse.b),
            glm::vec3(scene->mLights[IndexLight]->mColorSpecular.r,
                      scene->mLights[IndexLight]->mColorSpecular.g,
                      scene->mLights[IndexLight]->mColorSpecular.b),
            glm::vec3(LightNode->mParent->mTransformation[0][3],
                      LightNode->mParent->mTransformation[1][3],
                      LightNode->mParent->mTransformation[2][3]),
            glm::vec3(scene->mLights[IndexLight]->mDirection.x,
                      scene->mLights[IndexLight]->mDirection.y,
                      scene->mLights[IndexLight]->mDirection.z),
            scene->mLights[IndexLight]->mAngleOuterCone,
            scene->mLights[IndexLight]->mAngleInnerCone
          );
    light->SetName(scene->mLights[IndexLight]->mName.C_Str());
    
    LightTypes type;
    switch(scene->mLights[IndexLight]->mType)
    {
        case aiLightSource_POINT:
            type = LightTypes::Point;
        break;
        case aiLightSource_SPOT:
            type = LightTypes::Spot;
        break;
        default:
        case aiLightSource_DIRECTIONAL:
            type = LightTypes::Directional;
        break;
    }
    light->SetType(type);
    --IndexLight;
    return std::move(light);
}
std::unique_ptr<Camera> Loader::GetCamera()
{
    if (!Has_Camera())
        return nullptr;

    aiNode* CameraNode = scene->mRootNode->FindNode(scene->mCameras[0]->mName);
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
        );
    cam->SetName(scene->mCameras[0]->mName.C_Str());

    return std::move(cam);
}

std::unique_ptr<Model> Loader::GetModel(uint32_t Indx) 
{
    if (!scene && !scene->mRootNode && Indx == -1 && scene->mRootNode->mChildren[Indx]->mNumMeshes == 0)
        return nullptr;
    
    std::unique_ptr<Model> model = std::make_unique<Model>();
    model->SetName(scene->mRootNode->mChildren[Indx]->mName.C_Str());
    model->SetRoot(processNode(scene->mRootNode->mChildren[Indx]));
    return std::move(model);
}

std::shared_ptr<Node> Loader::processNode(aiNode* node)
{
    const uint16_t MatSize = 4;
    // process each mesh located at the current node
    std::shared_ptr<Node> curNode = std::make_shared<Node>();
    curNode->SetName(node->mName.C_Str());
    std::shared_ptr<glm::mat4> ModelMatr = std::make_shared<glm::mat4>(1.0f);
    for (uint16_t i = 0; i < MatSize * MatSize; ++i)//copy node's transform matr
        (*ModelMatr)[static_cast<uint16_t>(i / MatSize)][i % MatSize] = node->mTransformation[i % MatSize][i / MatSize];
    curNode->tr.Set(ModelMatr);
    //meshes.reserve(node->mNumMeshes);
    for (std::size_t i = 0; i < node->mNumMeshes; ++i)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        curNode->addMesh(processMesh(mesh));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (std::size_t i = 0; i < node->mNumChildren; ++i)
        curNode->addChild(processNode(node->mChildren[i]));
    return curNode;
}

std::shared_ptr <Mesh> Loader::processMesh(aiMesh* mesh)
{
    std::shared_ptr<Mesh> CurMesh = std::make_shared<Mesh>();
    // process materials
    aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

    //Create additional thread for load geometry and material of mesh
    std::thread GeometryThread([&]()
        {
            // walk through each of the mesh's vertices
            CurMesh->vertices.Positions.resize(Mesh::CardCoordsPerPoint * mesh->mNumVertices);
            CurMesh->vertices.Normals.resize(Mesh::CardCoordsPerPoint * mesh->mNumVertices);
            CurMesh->vertices.TexCoords.resize(Mesh::CardCoordsPerTextPoint * mesh->mNumVertices);
            CurMesh->vertices.Tangents.resize(Mesh::CardCoordsPerPoint * mesh->mNumVertices);
            CurMesh->vertices.Bitangents.resize(Mesh::CardCoordsPerPoint * mesh->mNumVertices);
            for (std::size_t i = 0; i < mesh->mNumVertices; ++i)
            {
                // positions
                CurMesh->vertices.Positions[Mesh::CardCoordsPerPoint * i] = mesh->mVertices[i].x;
                CurMesh->vertices.Positions[Mesh::CardCoordsPerPoint * i + 1] = mesh->mVertices[i].y;
                CurMesh->vertices.Positions[Mesh::CardCoordsPerPoint * i + 2] = mesh->mVertices[i].z;
                // normals
                CurMesh->vertices.Normals[Mesh::CardCoordsPerPoint * i] = mesh->mNormals[i].x;
                CurMesh->vertices.Normals[Mesh::CardCoordsPerPoint * i + 1] = mesh->mNormals[i].y;
                CurMesh->vertices.Normals[Mesh::CardCoordsPerPoint * i + 2] = mesh->mNormals[i].z;

                // texture coordinates
                if (mesh->HasTextureCoords(0)) // does the mesh contain texture coordinates?
                {
                    // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                    // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                    CurMesh->vertices.TexCoords[Mesh::CardCoordsPerTextPoint * i] = mesh->mTextureCoords[0][i].x;
                    CurMesh->vertices.TexCoords[Mesh::CardCoordsPerTextPoint * i + 1] = mesh->mTextureCoords[0][i].y;
                }
                else
                {
                    CurMesh->vertices.TexCoords[Mesh::CardCoordsPerTextPoint * i] = 0.0f;
                    CurMesh->vertices.TexCoords[Mesh::CardCoordsPerTextPoint * i + 1] = 0.0f;
                }
                if (mesh->HasTangentsAndBitangents())
                {
                    // tangent
                    CurMesh->vertices.Tangents[Mesh::CardCoordsPerPoint * i] = mesh->mTangents[i].x;
                    CurMesh->vertices.Tangents[Mesh::CardCoordsPerPoint * i + 1] = mesh->mTangents[i].y;
                    CurMesh->vertices.Tangents[Mesh::CardCoordsPerPoint * i + 2] = mesh->mTangents[i].z;
                    // bitangent
                    CurMesh->vertices.Bitangents[Mesh::CardCoordsPerPoint * i] = mesh->mBitangents[i].x;
                    CurMesh->vertices.Bitangents[Mesh::CardCoordsPerPoint * i + 1] = mesh->mBitangents[i].y;
                    CurMesh->vertices.Bitangents[Mesh::CardCoordsPerPoint * i + 2] = mesh->mBitangents[i].z;
                }
                else
                {
                    // tangent
                    CurMesh->vertices.Tangents[Mesh::CardCoordsPerPoint * i] =     0.0f;
                    CurMesh->vertices.Tangents[Mesh::CardCoordsPerPoint * i + 1] = 0.0f;
                    CurMesh->vertices.Tangents[Mesh::CardCoordsPerPoint * i + 2] = 0.0f;
                    // bitangent
                    CurMesh->vertices.Bitangents[Mesh::CardCoordsPerPoint * i] =     0.0f;
                    CurMesh->vertices.Bitangents[Mesh::CardCoordsPerPoint * i + 1] = 0.0f;
                    CurMesh->vertices.Bitangents[Mesh::CardCoordsPerPoint * i + 2] = 0.0f;
                }
            }
            // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.

            CurMesh->indices.reserve(mesh->mNumFaces * mesh->mFaces[0].mNumIndices);//Indices = 3 * card of face's number
            for (std::size_t i = 0; i < mesh->mNumFaces; ++i)
            {
                aiFace face = mesh->mFaces[i];
                // retrieve all indices of the face and store them in the indices vector
                for (std::size_t j = 0; j < face.mNumIndices; j++)
                    CurMesh->indices.push_back(face.mIndices[j]);
            }

            //load materials
            CurMesh->material.emplace_back(loadMaterial(mat, mesh->mMaterialIndex));
        });

    // Loading texture's maps
    std::vector< std::shared_ptr<Texture>> texes;
    // 1. diffuse maps
    texes = loadTexture(mat, aiTextureType_DIFFUSE, "texture_diffuse");
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 2. specular maps
    texes = loadTexture(mat, aiTextureType_SPECULAR, "texture_specular");
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 3. normal maps
    texes = loadTexture(mat, aiTextureType_NORMALS, "texture_normal");//aiTextureType_HEIGHT
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 4. height maps
    texes = loadTexture(mat, aiTextureType_AMBIENT, "texture_height");
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 5. emissive maps
    texes = loadTexture(mat, aiTextureType_EMISSIVE, "texture_emissive");
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    texes = loadTexture(mat, aiTextureType_UNKNOWN, "texture_metallic_rougness");
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    /*// 6. metallic maps
    texes = loadTexture(mat, aiTextureType_METALNESS, "texture_metallic");
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 7. roughness maps
    texes = loadTexture(mat, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness");
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());*/
    // 8. ambient_occlusion
    texes = loadTexture(mat, aiTextureType_AMBIENT_OCCLUSION, "texture_ambient_occlusion");
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());

    GeometryThread.join();

    CurMesh->setupMesh();
    // return a mesh object created from the extracted mesh data
    return CurMesh;
}

std::vector< std::shared_ptr<Texture>> Loader::loadTexture(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector< std::shared_ptr<Texture>> textures;
    aiString path, name;
    const void* TexturePtr;
    aiTexture* tex;
    std::string str;
    uint32_t Width, Height;

    textures.reserve(mat->GetTextureCount(type));
    for (std::size_t i = 0; i < mat->GetTextureCount(type); ++i)
    {
        Width = 0;
        Height = 0;
        mat->Get(AI_MATKEY_NAME, name);
        mat->Get(AI_MATKEY_TEXTURE(type, static_cast<unsigned int>(0)), path);

        if (scene->HasTextures())// textures are embedded in scene file
        {
             str = std::string(path.C_Str());//assimp docs, if tex embedded: path's string has format *TextureIndex
             tex = scene->mTextures[std::stoul(str.substr(1))];
             Width = tex->mWidth;
             Height = tex->mHeight;
             TexturePtr = tex->pcData;
        }
        else // textures place in external files 
        {
           //str = this->directory + '\\' + path.C_Str();
           TexturePtr = static_cast<const void*>(path.C_Str());
        }

        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        // if texture hasn't been loaded already, load it
        auto node = Mesh::GlobalTextures.find(std::hash<std::string>{}(static_cast<const char*>(TexturePtr)));
        if (node != Mesh::GlobalTextures.end())
             textures.push_back(Mesh::GlobalTextures.find(std::hash<std::string>{}(path.C_Str()))->second.lock());
        else
        {
            std::shared_ptr <Texture> texture = std::make_shared<Texture>();
            texture->id = TextureFromFile(TexturePtr, Width, Height, scene->HasTextures());
            texture->name = std::move(name);
            texture->path = std::move(path);
            texture->type = typeName;

            textures.push_back(texture);
            Mesh::GlobalTextures[std::hash<std::string>{}(texture->path.C_Str())] = texture;// store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }     
    }
    return std::move(textures);
}

Material Loader::loadMaterial(aiMaterial* mat, uint16_t indx)
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

GLuint Loader::TextureFromFile(const void* path, std::size_t width, std::size_t height, bool FromProc)
{
    GLuint textureID = 0;
    const unsigned char* data;
    int w, h, nrComponents = 4;
    bool Loaded = false;

    if (FromProc)
    {
        if (height == 0)
        {
            data = static_cast<const unsigned char*>(stbi_load_from_memory((unsigned char*)path, static_cast<int>(width), &w, &h, &nrComponents, 0));
            //std::cout << stbi_failure_reason() << std::endl;
            Loaded = true;
        }
        else
        {
            data = static_cast<const unsigned char*>(path);
            w = static_cast<int>(width);
            h = static_cast<int>(height);
        }
    }
    else
    {
        data = stbi_load(static_cast<const char*>(path), &w, &h, &nrComponents, 0);
        Loaded = true;
    }

    if (data)
    {
        GLenum format = GL_RGB32F, mipfor = GL_RGB;
        if (nrComponents == 1)
        {
            mipfor = GL_RED;
            format = GL_R16F;
        }
        else if (nrComponents == 3)
        {
            format = GL_RGB32F;
            mipfor = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            format = GL_RGBA32F;
            mipfor = GL_RGBA;
        }

        glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
        glGenerateTextureMipmap(textureID);

        glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTextureStorage2D(textureID, 1, format, w, h);
        glTextureSubImage2D(textureID, 0, 0, 0, w, h, mipfor, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
    }
    if (Loaded)
        stbi_image_free(const_cast<unsigned char*>(data));

    return textureID;
}

GLuint Loader::CubeTextureFromFile(std::vector<std::string_view> paths)
{
    GLuint textureID = 0;

    int width, height, nrComponents;
    unsigned char* data;

    glCreateTextures(GL_TEXTURE_CUBE_MAP_ARRAY, 1, &textureID);

    data = stbi_load(paths[0].data(), &width, &height, &nrComponents, 0);
    glTextureStorage3D(textureID, 1, GL_RGB32F, width, height, 6);
    glTextureSubImage3D(textureID, 0, 0, 0, 0, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, data);

    for (std::size_t i = 1; i < paths.size(); ++i)
    {
        data = stbi_load(paths[i].data(), &width, &height, &nrComponents, 0);

        if (data)
        {
            GLenum format = GL_RGB32F, mipfor = GL_RGB;
            if (nrComponents == 1)
            {
                mipfor = GL_RED;
                format = GL_R16F;
            }
            else if (nrComponents == 3)
            {
                format = GL_RGB32F;
                mipfor = GL_RGB;
            }
            else if (nrComponents == 4)
            {
                format = GL_RGBA32F;
                mipfor = GL_RGBA;
            }

            glTextureSubImage3D(textureID, 0, 0, 0, static_cast<GLint>(i), width, height, 1, mipfor, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cerr << "Texture failed to load at path " << std::endl;
        }
    }
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureID;
}

std::unique_ptr<Node> Loader::LoadSkyBox(std::vector<std::string_view> paths)
{
    std::unique_ptr<Node> curNode = std::make_unique<Node>();
    curNode->SetName("SkyBox");
    std::shared_ptr<Mesh> curMesh = std::make_shared<Mesh>();
    //Cube's vertices
    curMesh->vertices.Positions = std::vector
    (   
        { 
            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f
        }
    );
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
    curMesh->vertices.Normals.resize(0);
    curMesh->vertices.TexCoords.resize(0);
    curMesh->vertices.Tangents.resize(0);
    curMesh->vertices.Bitangents.resize(0);

    std::shared_ptr <Texture> texture = std::make_shared<Texture>();
    texture->id = CubeTextureFromFile(paths);
    texture->name = "SkyBox";
    texture->path = paths[0].data();
    texture->type = "SkyBox";

    curMesh->textures.push_back(texture);
    Mesh::GlobalTextures[std::hash<std::string>{}(texture->path.C_Str())] = texture;

    curMesh->setupMesh();
    curNode->addMesh(curMesh);

    return std::move(curNode);
}


Loader::~Loader() 
{
}

std::unique_ptr<Scene> Loader::GetScene(std::string_view path)
{
    std::unique_ptr<Scene> scen = std::make_unique<Scene>();
    LoadScene(path);

    if (!Is_Load())
        throw(std::string("Problem with scene loading!"));
    if (Has_Camera())
        scen->SetCam(GetCamera());

    std::shared_ptr <Light> light;
    if (!Has_Light())
    {
        //default light
        light = std::make_shared<Light>(0.6f, 0.09f, 0.032f,
            glm::vec3(0.1f, 0.1f, 0.1f),
            glm::vec3(0.8f, 0.8f, 0.8f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(-1.5f, 10.0f, 0.0f));
        light->SetType(LightTypes::Point);
        light->SetName("Default light");
        scen->AddLight(light);

        Scene::DefaultPointLightModel->GetRoot()->tr.Set(light->tr.Get());
        scen->AddModel(Scene::DefaultPointLightModel);
    }
    else
        while ((light = std::shared_ptr<Light>(GetLight().release())))
        {
            scen->AddLight(light);
            Scene::DefaultPointLightModel->GetRoot()->tr.Set(light->tr.Get());
            scen->AddModel(Scene::DefaultPointLightModel);
        }

        //std::list<std::thread> threads;// (new std::thread[IndexModel + 1]);
        //std::unique_ptr<std::future<std::unique_ptr<Model>>[]> results(new std::future <std::unique_ptr<Model>>[IndexModel + 1]);
        //auto AdMod = [&](int32_t n) {  scen->AddModel(std::shared_ptr<Model>(GetModel(n).release())); };
        //for (int32_t i = 0; i <= IndexModel; ++i)
        //    threads.push_back(std::thread(AdMod,i));
        //for (auto& th : threads)
        //    th.join();
        //    results[i] = std::async(std::launch::async, AdMod, i);
    std::shared_ptr<Shader> sh = scen->GetShader("Default");
    for (int32_t i = 0; i <= IndexModel; ++i)
    {
        std::unique_ptr<Model> mod = GetModel(i);
        mod->SetShader(sh);
        scen->AddModel(std::shared_ptr<Model>(mod.release()));
    }
    //std::shared_ptr<Model> mod;
    //while ((mod = std::shared_ptr<Model>(GetModel().release())))
    //   scen->AddModel(mod);

    return std::move(scen);
}