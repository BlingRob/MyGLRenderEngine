#pragma once
#include <array>
#include "assimp/scene.h"
#include "Loaders/ImageLoader.h"

struct Vertexes
{
    enum PointTypes
    {
        positions = 0, normals, texture, tangent, bitangent
    };
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

enum class Texture_Types:uint64_t {Diffuse = 0, Normal = 1, Specular = 2, Emissive = 3, Height = 4, Metallic_roughness = 5, Ambient_occlusion = 6, Skybox = 7};

struct BaseTexture
{
    std::vector<std::shared_ptr<Image>> imgs;
    Texture_Types type;
    std::string name;
    std::string path;
};

struct Material
{
    std::uint32_t id;
    aiColor3D ambient;
    aiColor3D diffuse;
    aiColor3D specular;
    float shininess;
};


