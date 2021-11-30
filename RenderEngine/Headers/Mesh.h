#pragma once
#include "Shader.h"
#include "Material.h"

class BaseMesh: public Entity
{
public:
    /*  Mesh Data  */
    Vertexes vertices;
    Material material;
    /*  Functions  */
    virtual ~BaseMesh() {};
    /*  Functions    */
    void virtual setupMesh() {};
    void virtual Draw(const Shader* shader) {};
    /* Mesh's constants */
    static const std::size_t CardCoordsPerPoint = 3;
    static const std::size_t CardCoordsPerTextPoint = 2;
private:
    virtual void PureVirtual() = delete;
};

