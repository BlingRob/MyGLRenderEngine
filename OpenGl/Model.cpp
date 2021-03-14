#include "Model.h"

void Model::Draw(Shader* shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(std::string path)
{
    // read file via ASSIMP
    //Assimp::Importer importer;
    //const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
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
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    meshes.reserve(node->mNumMeshes);
    for (std::size_t i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
        processNode(node->mChildren[i], scene);
}

std::vector<Texture> Model::loadTexture(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    aiString str;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data, str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory,false);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    return std::move(textures);
}

std::vector<Material> Model::loadMaterial(aiMaterial* mat, uint16_t num)
{
    std::vector<Material> mats(num);
    uint16_t i = 0;

    for (auto& el : mats)
    {
        el.id = i++;
        mat->Get(AI_MATKEY_COLOR_AMBIENT, el.ambient);
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, el.diffuse);
        mat->Get(AI_MATKEY_COLOR_SPECULAR, el.specular);
        mat->Get(AI_MATKEY_SHININESS, el.shininess);
    }

    return std::move(mats);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    std::vector<Material> materials;

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
            // tangent
            vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            // bitangent
            vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        }
        else
            vertices[i].TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        indices.reserve(indices.size() + face.mNumIndices);
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
    std::vector<Texture> diffuseMaps(loadTexture(mat, aiTextureType_DIFFUSE, "texture_diffuse"));
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture> specularMaps(loadTexture(mat, aiTextureType_SPECULAR, "texture_specular"));
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());;
    // 3. normal maps
    std::vector<Texture> normalMaps(loadTexture(mat, aiTextureType_HEIGHT, "texture_normal"));
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    //textures.emplace_back(loadTexture(mat, aiTextureType_HEIGHT, "texture_normal"));
    // 4. height maps
    std::vector<Texture> heightMaps(loadTexture(mat, aiTextureType_AMBIENT, "texture_height"));
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    //textures.emplace_back(loadTexture(mat, aiTextureType_AMBIENT, "texture_height"));
    // 5. emissive maps
    std::vector<Texture> emissiveMaps(loadTexture(mat, aiTextureType_EMISSIVE, "texture_emissive"));
    textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
    //textures.emplace_back(loadTexture(mat, aiTextureType_EMISSIVE, "texture_emissive"));

    //load materials
    materials = loadMaterial(mat, scene->mNumMaterials);

    // return a mesh object created from the extracted mesh data
    return Mesh(std::move(vertices), std::move(indices), std::move(textures),std::move(materials));
}

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '\\' + filename;

    GLuint textureID;
    GLenum err;
    
    //glGenTextures(1, &textureID);

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

        //glBindTexture(GL_TEXTURE_2D, textureID);
        glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
       
        //glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        //glGenerateMipmap(GL_TEXTURE_2D);
        glGenerateTextureMipmap(textureID);

        glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTextureStorage2D(textureID, 1, format, width, height);
        glTextureSubImage2D(textureID, 0, 0, 0, width, height, mipfor, GL_UNSIGNED_BYTE, data);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}




