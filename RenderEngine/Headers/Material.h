#pragma once
#include <array>
#include <bitset>
#include "assimp/scene.h"
#include "Loaders/ImageLoader.h"
#include "Entity.h"

class Vertexes
{
public:
    enum PointTypes
    {
        positions = 0, normals, texture, tangent, bitangent
    };

    Vertexes(std::size_t size = 0, const float* positions = nullptr, std::vector<std::uint32_t>&& inds = std::vector<std::uint32_t>(0), const float* normals = nullptr, const float* textureCoords = nullptr, const float* tangent = nullptr, const float* bitangent = nullptr);
    Vertexes(Vertexes&&);
    Vertexes&& operator=(Vertexes&&);
    void AddIndices(std::vector<std::uint32_t>&& inds);
    std::size_t GetSize();
    std::vector<std::uint32_t>& GetIndices();
    const float* Get(PointTypes type);
    bool Has(PointTypes type);
private:
    std::bitset<5> HasPointType{0x0};
    std::size_t VectorsSize;
    std::array<const float*, 5> vectors;
    std::vector<std::uint32_t> indices;
};

enum class Texture_Types:uint8_t {Diffuse = 0, Normal = 1, Specular = 2, Emissive = 3, Height = 4, Metallic_roughness = 5, Ambient_occlusion = 6, Skybox = 7};

struct BaseTexture:public Entity
{
    BaseTexture(std::string& name, std::string& path, Texture_Types type, std::vector<std::shared_ptr<Image>> images);
    virtual ~BaseTexture() {};
    virtual bool createTexture() = 0;
    bool IsCreated();
    unsigned int GetId();
    std::string GetPath();
    Texture_Types GetType();
    std::vector<std::shared_ptr<Image>> GetImages();
    void AddImage(std::shared_ptr<Image> img);
protected:
    bool Created{false};
    unsigned int id;
    std::vector<std::shared_ptr<Image>> _imgs;
    Texture_Types _type;
    std::string _path;
private:

};

struct Material
{
    Material():id(0), ambient(0), diffuse(0), specular(0), shininess(0){}
    std::uint32_t id;
    aiColor3D ambient;
    aiColor3D diffuse;
    aiColor3D specular;
    float shininess;
};