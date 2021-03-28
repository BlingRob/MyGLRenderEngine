#pragma once
#include "Headers.h"
#include "Shader.h"


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

struct Texture
{
    GLuint id;
    std::string type;
    aiString name;
    aiString path;
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
    static inline std::map<std::size_t, std::weak_ptr<Texture>> GlobalTextures;
    Vertexes vertices;
    std::vector< std::shared_ptr<Texture>> textures;
    std::vector<GLuint> indices;
    Material material;
    /*  Functions  */
    //Mesh(Vertexes&& vertices, std::vector<GLuint>&& indices, std::vector < Texture>&& textures, Material&& material);
    /*  Functions    */
    void setupMesh();
    void Draw(Shader* shader);
private:
    /*  Render data  */
    GLuint VAO, VBO[5], EBO;
};


//std::map<std::size_t, std::weak_ptr<Texture>> Mesh::GlobalTextures;

