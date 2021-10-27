#pragma once
#include "Headers.h"
#include "Shader.h"
#include <assimp/scene.h>
#include "STB_Loader.h"
#include "ParallelStructures.h"

struct Vertexes
{
    // positions
    std::vector < GLfloat> Positions;
    // normals
    std::vector < GLfloat> Normals;
    // texCoords
    std::vector < GLfloat> TexCoords;
    // tangents
    std::vector < GLfloat> Tangents;
    // bitangents
    std::vector < GLfloat> Bitangents;
};

enum class Texture_Types:GLuint {Diffuse = 0, Normal = 1, Specular = 2, Emissive = 3, Height = 4, Metallic_roughness = 5, Ambient_occlusion = 6, Skybox = 7};

struct Texture
{
    std::variant<GLuint, std::unique_ptr<STB_Loader>> id; //Texture created in graphics API:Yes - there is index, no - there is buffer with bin data
    Texture_Types type;
    aiString name;
    aiString path;
    ~Texture()
    {
        glDeleteTextures(1, &std::get<GLuint>(id));
    }
};

struct Material 
{
    GLuint id;
    aiColor3D ambient;
    aiColor3D diffuse;
    aiColor3D specular;
    float shininess;
};

class Mesh
{
    
public:
    /*  Mesh Data  */
    static inline ParallelMap<std::size_t, std::weak_ptr<Texture>> GlobalTextures;
    Vertexes vertices;
    std::vector< std::shared_ptr<Texture>> textures;
    std::vector<GLuint> indices;
    Material material;
    /*  Functions  */
    //Mesh(Vertexes&& vertices, std::vector<GLuint>&& indices, std::vector < Texture>&& textures, Material&& material);
    ~Mesh();
    /*  Functions    */
    void setupMesh();
    void Draw(const Shader* shader);
    /* Mesh's constants */
    static const std::size_t CardCoordsPerPoint = 3;
    static const std::size_t CardCoordsPerTextPoint = 2;
private:
    /*  Render data  */
    GLuint VAO, VBO, EBO;
};

