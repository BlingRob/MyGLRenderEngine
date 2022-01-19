#pragma once
#include "../Mesh.h"
#include "gl/gl.h"
#include <numeric>

struct OGLTexture:BaseTexture
{
    OGLTexture(std::string& name, std::string& path, Texture_Types type, std::vector<std::shared_ptr<Image>> images):
        BaseTexture(name,path, type, std::move(images))
    {}
    bool createTexture() override;
    ~OGLTexture() override;
private:
    std::pair<GLenum, GLenum> Format(int channels);
};

class OGLMesh: public BaseMesh 
{
public:
    OGLMesh(std::unique_ptr<Vertexes> verts, std::unique_ptr<Material> mat = nullptr, std::vector<std::shared_ptr<OGLTexture>>&& texes = std::vector<std::shared_ptr<OGLTexture>>(0))
        :BaseMesh(std::move(verts), std::move(mat), std::forward<std::vector<std::shared_ptr<BaseTexture>>>(casting(std::forward<std::vector<std::shared_ptr<OGLTexture>>>(texes)))) {};
    /*  Functions  */
    ~OGLMesh() override;
    /*  Functions    */
    void setupMesh() override;
    void Draw(const Shader* shader) override;
private:
    /*  Render data  */
    GLuint VAO, VBO, EBO;
    //Help function
    std::vector<std::shared_ptr<BaseTexture>> casting(std::vector<std::shared_ptr<OGLTexture>>&& vec1)
    {
        std::vector<std::shared_ptr<BaseTexture>> out;
        for (auto& el : vec1)
            out.emplace_back(std::dynamic_pointer_cast<BaseTexture>(std::move(el)));
        return out;
    };
};

using Mesh = OGLMesh;
using Texture = OGLTexture;