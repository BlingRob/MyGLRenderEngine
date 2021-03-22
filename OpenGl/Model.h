#pragma once

#include "Headers.h"
#include "Mesh.h"

//GLuint TextureFromExternalFile(const char* path, const std::string& directory, bool gamma);
GLuint TextureFromFile(const void* path, std::size_t width, std::size_t height, bool FromProc);
class Model
{
public:
    /*  Методы   */
    Model(const char* path)
    {
        loadModel(path);
    }
    void Draw(Shader* shader);
private:
    /*  Данные модели  */
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh>    meshes;
    std::string directory;
    bool gammaCorrection;
    /*  Методы   */
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadTexture(aiMaterial* mat, aiTexture** tex, aiTextureType type, std::string typeName, bool embedded);
    Material loadMaterial(aiMaterial* mat,uint16_t indx);
};