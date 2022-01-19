#pragma once
#include "Shader.h"
#include "Material.h"

class BaseMesh: public Entity
{
public:
    BaseMesh(std::unique_ptr<Vertexes> verts, std::unique_ptr<Material> mat = nullptr, std::vector<std::shared_ptr<BaseTexture>>&& texes = std::vector<std::shared_ptr<BaseTexture>>(0));
    /*  Functions  */
    virtual ~BaseMesh() {}
    void virtual setupMesh() = 0;
    void virtual Draw(const Shader* shader) = 0;
    void SetMaterial(std::unique_ptr<Material> mat);
    void SetVertexes(std::unique_ptr<Vertexes> verts);
    void AddTexture(std::shared_ptr<BaseTexture> tex);
    /* Mesh's constants */
    static const std::size_t CardCoordsPerPoint = 3;
    static const std::size_t CardCoordsPerTextPoint = 2;
protected:
    /*  Mesh Data  */
    std::unique_ptr<Vertexes> vertices;
    std::unique_ptr<Material> material;
    /* Textures */
    std::vector<std::shared_ptr<BaseTexture>> textures;
};
