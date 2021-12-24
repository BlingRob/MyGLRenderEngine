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
    std::array<bool, 5> HasPointType{false};
    std::size_t VectorsSize;
    std::array<const float*, 5> vectors;
    std::vector<std::uint32_t> indices;
};

enum class Texture_Types:uint8_t {Diffuse = 0, Normal = 1, Specular = 2, Emissive = 3, Height = 4, Metallic_roughness = 5, Ambient_occlusion = 6, Skybox = 7};

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


