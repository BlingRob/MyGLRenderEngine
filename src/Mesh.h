#pragma once
#include "Headers.h"
#include "Shader.h"
#include <assimp/scene.h>
#include "STB_Loader.h"
#include "ParallelStructures.h"

struct Vertexess
{
    // positions
    std::vector <GLfloat> Positions;
    // normals
    std::vector <GLfloat> Normals;
    // texCoords
    std::vector <GLfloat> TexCoords;
    // tangents
    std::vector <GLfloat> Tangents;
    // bitangents
    std::vector <GLfloat> Bitangents;
};

struct Vertexes
{
    enum PointTypes
    {
        positions = 0, normals, texture, tangent, bitangent
    };
    //Vertexess(std::array<std::size_t, 5> sizes, float* pos, float* nor, float* tc, float* tan, float* bits) :
    //    _msizes(std::move(sizes)), Positions(pos), Normals(nor), TexCoords(tc), Tangents(tc), Bitangents(bits) {}
    std::array<std::size_t, 5> _msizes{0, 0, 0, 0, 0};
    // positions
    aiVector3D* Positions;
    // normals
    aiVector3D* Normals;
    // texCoords
    aiVector3D* TexCoords;
    // tangents
    aiVector3D* Tangents;
    // bitangents
    aiVector3D* Bitangents;
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

