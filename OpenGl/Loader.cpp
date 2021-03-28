#include "Loader.h"

bool Loader::LoadScene(const std::string& path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    importer.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    scene = std::unique_ptr<aiScene>(importer.GetOrphanedScene());
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        error = "ERROR::ASSIMP ";
        return false;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('\\'));
    IndexModel = scene->mRootNode->mNumChildren - 1;
    IndexLight = scene->mNumLights - 1;

    return true;
}

void Loader::Destroy() 
{
    delete scene.release();
    scene.reset(nullptr);
}

Loader::Loader(const std::string& path) 
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
    if (LightNode != nullptr) 
    {
        scene->mLights[IndexLight]->mPosition.x = LightNode->mParent->mTransformation[0][3];
        scene->mLights[IndexLight]->mPosition.y = LightNode->mParent->mTransformation[1][3];
        scene->mLights[IndexLight]->mPosition.z = LightNode->mParent->mTransformation[2][3];
    }
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
            glm::vec3(scene->mLights[IndexLight]->mPosition.x,
                      scene->mLights[IndexLight]->mPosition.y,
                      scene->mLights[IndexLight]->mPosition.z),
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

std::unique_ptr<Model> Loader::GetModel() 
{
    if (!scene->mRootNode || IndexModel == -1)
            return nullptr;
    
    std::unique_ptr<Model> model = std::make_unique<Model>();
    while(scene->mRootNode->mChildren[IndexModel]->mNumMeshes == 0)//Skip camera or lights nodes
        --IndexModel;
    model->SetName(scene->mRootNode->mChildren[IndexModel]->mName.C_Str());
    model->SetRoot(processNode(scene->mRootNode->mChildren[IndexModel]));
    --IndexModel;
    return std::move(model);
}

std::shared_ptr<Node> Loader::processNode(aiNode* node)
{
    const uint16_t MatSize = 4;
    // process each mesh located at the current node
    std::shared_ptr<Node> curNode = std::make_shared<Node>();
    curNode->SetName(node->mName.C_Str());
    for (uint16_t i = 0; i < MatSize * MatSize; ++i)//copy node's transform matr
        curNode->ModelMatrix[static_cast<uint16_t>(i / MatSize)][i % MatSize] = node->mTransformation[i % MatSize][i / MatSize];
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

    // walk through each of the mesh's vertices
    /*
    CurMesh->vertices.Positions.reserve(mesh->mNumVertices);
    CurMesh->vertices.Normals.reserve(mesh->mNumVertices);
    CurMesh->vertices.TexCoords.reserve(mesh->mNumVertices);
    CurMesh->vertices.Tangents.reserve(mesh->mNumVertices);
    CurMesh->vertices.Bitangents.reserve(mesh->mNumVertices);
    */
    const uint16_t CardCoordsPerPoint = 3;
    const uint16_t CardCoordsPerTextPoint = 2;
    CurMesh->vertices.Positions.resize(CardCoordsPerPoint * mesh->mNumVertices);
    CurMesh->vertices.Normals.resize(CardCoordsPerPoint * mesh->mNumVertices);
    CurMesh->vertices.TexCoords.resize(CardCoordsPerTextPoint * mesh->mNumVertices);
    CurMesh->vertices.Tangents.resize(CardCoordsPerPoint * mesh->mNumVertices);
    CurMesh->vertices.Bitangents.resize(CardCoordsPerPoint * mesh->mNumVertices);
    for (std::size_t i = 0; i < mesh->mNumVertices; ++i)
    {
        // positions
        CurMesh->vertices.Positions[CardCoordsPerPoint * i] = mesh->mVertices[i].x;
        CurMesh->vertices.Positions[CardCoordsPerPoint * i + 1] = mesh->mVertices[i].y;
        CurMesh->vertices.Positions[CardCoordsPerPoint * i + 2] = mesh->mVertices[i].z;
        //CurMesh->vertices.Positions.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
        // normals
        CurMesh->vertices.Normals[CardCoordsPerPoint * i] = mesh->mNormals[i].x;
        CurMesh->vertices.Normals[CardCoordsPerPoint * i + 1] = mesh->mNormals[i].y;
        CurMesh->vertices.Normals[CardCoordsPerPoint * i + 2] = mesh->mNormals[i].z;
        //CurMesh->vertices.Normals.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
        // texture coordinates
        
        if (mesh->HasTextureCoords(0)) // does the mesh contain texture coordinates?
        {
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            //CurMesh->vertices.TexCoords.push_back(glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
            CurMesh->vertices.TexCoords[CardCoordsPerTextPoint * i] = mesh->mTextureCoords[0][i].x;
            CurMesh->vertices.TexCoords[CardCoordsPerTextPoint * i + 1] = mesh->mTextureCoords[0][i].y;
        }
        else
        {
           // CurMesh->vertices.TexCoords.push_back(glm::vec2(0.0f, 0.0f));
            CurMesh->vertices.TexCoords[CardCoordsPerTextPoint * i] = 0.0f;
            CurMesh->vertices.TexCoords[CardCoordsPerTextPoint * i + 1] = 0.0f;
        }
        if (mesh->HasTangentsAndBitangents())
        {
            // tangent
            //CurMesh->vertices.Tangents.push_back(glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z));
            CurMesh->vertices.Tangents[CardCoordsPerPoint * i] = mesh->mTangents[i].x;
            CurMesh->vertices.Tangents[CardCoordsPerPoint * i + 1] = mesh->mTangents[i].y;
            CurMesh->vertices.Tangents[CardCoordsPerPoint * i + 2] = mesh->mTangents[i].z;
            // bitangent
            CurMesh->vertices.Bitangents[CardCoordsPerPoint * i] = mesh->mBitangents[i].x;
            CurMesh->vertices.Bitangents[CardCoordsPerPoint * i + 1] = mesh->mBitangents[i].y;
            CurMesh->vertices.Bitangents[CardCoordsPerPoint * i + 2] = mesh->mBitangents[i].z;
            //CurMesh->vertices.Bitangents.push_back(glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z));
        }
        else
        {
            // tangent
            //CurMesh->vertices.Tangents.push_back(glm::vec3(0, 0, 0));
            // bitangent
            //CurMesh->vertices.Bitangents.push_back(glm::vec3(0, 0, 0));
                        // tangent
            //CurMesh->vertices.Tangents.push_back(glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z));
            CurMesh->vertices.Tangents[CardCoordsPerPoint * i] = 0.0f;
            CurMesh->vertices.Tangents[CardCoordsPerPoint * i + 1] = 0.0f;
            CurMesh->vertices.Tangents[CardCoordsPerPoint * i + 2] = 0.0f;
            // bitangent
            CurMesh->vertices.Bitangents[CardCoordsPerPoint * i] = 0.0f;
            CurMesh->vertices.Bitangents[CardCoordsPerPoint * i + 1] = 0.0f;
            CurMesh->vertices.Bitangents[CardCoordsPerPoint * i + 2] = 0.0f;
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
    // process materials
    aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    //auto Merge = [&](std::vector<Texture>& v1, std::vector<Texture>& v2) {for (auto& el : v2)v1.emplace_back(el); };
    std::vector< std::shared_ptr<Texture>> texes;
    // 1. diffuse maps
    texes = loadTexture(mat, aiTextureType_DIFFUSE, "texture_diffuse");
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 2. specular maps
    texes = loadTexture(mat, aiTextureType_SPECULAR, "texture_specular");
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 3. normal maps
    texes = loadTexture(mat, aiTextureType_HEIGHT, "texture_normal");
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 4. height maps
    texes = loadTexture(mat, aiTextureType_AMBIENT, "texture_height");
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());
    // 5. emissive maps
    texes = loadTexture(mat, aiTextureType_EMISSIVE, "texture_emissive");
    CurMesh->textures.insert(CurMesh->textures.end(), texes.begin(), texes.end());

    //load materials
    CurMesh->material = loadMaterial(mat, mesh->mMaterialIndex);

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
        mat->Get(AI_MATKEY_NAME(type, i), name);
        mat->Get(AI_MATKEY_TEXTURE(type, static_cast<unsigned int>(i)), path);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        // if texture hasn't been loaded already, load it
        auto node = Mesh::GlobalTextures.find(std::hash<std::string>{}(path.C_Str()));
        if(node != Mesh::GlobalTextures.end())
            textures.push_back(Mesh::GlobalTextures.find(std::hash<std::string>{}(path.C_Str()))->second.lock());
        else
        {
            
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
                str = this->directory + '\\' + path.C_Str();
                TexturePtr = static_cast<const void*>(str.c_str());
            }
  
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
    mat->Get(AI_MATKEY_COLOR_AMBIENT(indx), MeshMaterial.ambient);
    mat->Get(AI_MATKEY_COLOR_DIFFUSE(indx), MeshMaterial.diffuse);
    mat->Get(AI_MATKEY_COLOR_SPECULAR(indx), MeshMaterial.specular);
    mat->Get(AI_MATKEY_SHININESS(indx), MeshMaterial.shininess);

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

    //unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
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
        glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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

Loader::~Loader() 
{
}