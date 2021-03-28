#include "Model.h"

void Model::Draw()
{
    mRootNode->Draw(shader);
}

void Model::SetRoot(std::shared_ptr<Node> root)
{
    mRootNode = root;
}

std::string Model::GetName() const
{
    return name;
}
void Model::SetName(std::string name)
{
    this->name = name;
}

void Model::SetShader(std::shared_ptr <Shader> sh) 
{
    shader = sh;
}
/*
// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(const std::string& path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    //const aiScene* scene1 = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cerr << "ERROR::ASSIMP " << std::endl;
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('\\'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
    importer.FreeScene();
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    meshes.reserve(meshes.size() + node->mNumMeshes);
    for (std::size_t i = 0; i < node->mNumMeshes; ++i)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (std::size_t i = 0; i < node->mNumChildren; ++i)
        processNode(node->mChildren[i], scene);
}

std::vector<Texture> Model::loadTexture(aiMaterial* mat,aiTexture** texes, aiTextureType type, std::string typeName,bool embedded)
{
    std::vector<Texture> textures;
    aiString path,name;
    const void* TexturePtr;
    aiTexture* tex;
    std::string str;
    uint32_t Width,Height;

    for (std::size_t i = 0; i < mat->GetTextureCount(type); ++i)
    {
        Width = 0;
        Height = 0;
        //mat->GetTexture(type, 1, &path);
        mat->Get(AI_MATKEY_TEXTURE(type, i), path);
        mat->Get(AI_MATKEY_NAME(type, i), name);
        if (embedded) 
        {
            str = std::string(path.C_Str());//assimp docs, if tex embedded: path's string has format *TextureIndex
            tex = texes[std::stoul(str.substr(1))];
            //str = std::to_string(reinterpret_cast<uint32_t>(tex->pcData));
            Width = tex->mWidth;
            Height = tex->mHeight;
            TexturePtr = tex->pcData;
        }
        else
        {
            str = this->directory + '\\' + path.C_Str();
            TexturePtr = static_cast<const void*>(str.c_str());
        }
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture

        bool skip = false;
        for (std::size_t j = 0; j < textures_loaded.size(); ++j)
        {
            if (std::strcmp(textures_loaded[j].name.data, name.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(TexturePtr, Width, Height, embedded);
            texture.name = std::move(name);
            texture.type = typeName;
            //texture.path = str.C_Str();
            //texture.id = TextureFromExternalFile(str.C_Str(), this->directory, false);
           
            
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    return std::move(textures);
}

Material Model::loadMaterial(aiMaterial* mat, uint16_t indx)
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

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
    Material material;

    // walk through each of the mesh's vertices
    vertices.reserve(mesh->mNumVertices);
    for (std::size_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // positions
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        // normals
        vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else
            vertices[i].TexCoords = glm::vec2(0.0f, 0.0f);
        if (mesh->mTangents != nullptr && mesh->mBitangents != nullptr) 
        {
            // tangent
            vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            // bitangent
            vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        }
        else 
        {
            // tangent
            vertex.Tangent = glm::vec3(0, 0, 0);
            // bitangent
            vertex.Bitangent = glm::vec3(0, 0, 0);
        }

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.

    indices.reserve(mesh->mNumFaces * mesh->mFaces[0].mNumIndices);//Indices = 3 * card of face's number
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (std::size_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    //textures.emplace_back(loadTexture(mat, aiTextureType_DIFFUSE, "texture_diffuse"));
    std::vector<Texture> diffuseMaps(loadTexture(mat, scene->mTextures, aiTextureType_DIFFUSE, "texture_diffuse",scene->mNumTextures != 0));
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture> specularMaps(loadTexture(mat, scene->mTextures, aiTextureType_SPECULAR, "texture_specular", scene->mNumTextures != 0));
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());;
    // 3. normal maps
    std::vector<Texture> normalMaps(loadTexture(mat, scene->mTextures, aiTextureType_HEIGHT, "texture_normal", scene->mNumTextures != 0));
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    //textures.emplace_back(loadTexture(mat, aiTextureType_HEIGHT, "texture_normal"));
    // 4. height maps
    std::vector<Texture> heightMaps(loadTexture(mat, scene->mTextures, aiTextureType_AMBIENT, "texture_height", scene->mNumTextures != 0));
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    //textures.emplace_back(loadTexture(mat, aiTextureType_AMBIENT, "texture_height"));
    // 5. emissive maps
    std::vector<Texture> emissiveMaps(loadTexture(mat, scene->mTextures, aiTextureType_EMISSIVE, "texture_emissive", scene->mNumTextures != 0));
    textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
    //textures.emplace_back(loadTexture(mat, aiTextureType_EMISSIVE, "texture_emissive"));

    //load materials
    material = loadMaterial(mat, mesh->mMaterialIndex);

    // return a mesh object created from the extracted mesh data
    return Mesh(std::move(vertices), std::move(indices), std::move(textures),std::move(material));
}

GLuint TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '\\' + filename;

    GLuint textureID;

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
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

        glTextureStorage2D(textureID, 1, format, width, height);
        glTextureSubImage2D(textureID, 0, 0, 0, width, height, mipfor, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}*/

/*GLuint TextureFromFile(const void* path,std::size_t width, std::size_t height, bool FromProc)
{
    GLuint textureID;
    const unsigned char* data;
    int w, h, nrComponents = 4;
    bool Loaded = false;

    if (FromProc) 
    {
        if (height == 0)
        {
            data = static_cast<const unsigned char*>(stbi_load_from_memory((unsigned char*)path, width, &w, &h, &nrComponents, 0));
            //std::cout << stbi_failure_reason() << std::endl;
            Loaded = true;
        }
        else
        {
            data = static_cast<const unsigned char*>(path);
            w = width;
            h = height;
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
    if(Loaded)
        stbi_image_free(const_cast<unsigned char*>(data));

    return textureID;
}*/



