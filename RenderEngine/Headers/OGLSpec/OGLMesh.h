#pragma once
#include "../Mesh.h"
#include "gl/gl.h"
#include <numeric>

struct OGLTexture:BaseTexture
{
    GLuint id;
    bool createGLTex();
    bool IsCreated();
    ~OGLTexture()
    {
        glDeleteTextures(1, &id);
    }
private:
    bool Created;
    std::pair<GLenum, GLenum> Format(int channels);
};

class OGLMesh: public BaseMesh 
{
public:
    std::vector<std::shared_ptr<OGLTexture>> textures;
    std::vector<GLuint> indices;
    /*  Functions  */
    ~OGLMesh();
    /*  Functions    */
    void setupMesh() override;
    void Draw(const Shader* shader) override;
private:
    /*  Render data  */
    GLuint VAO, VBO, EBO;
};

using Mesh = OGLMesh;
using Texture = OGLTexture;