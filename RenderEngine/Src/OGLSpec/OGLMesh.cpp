#include "../../Headers/OGLSpec/OGLMesh.h"

inline std::pair<GLenum, GLenum> OGLTexture::Format(int channels) 
{
    std::pair<GLenum, GLenum> formats = std::make_pair(GL_RGB32F, GL_RGB);
    switch (channels)
    {
    case 1:
        formats.first = GL_R8;
        formats.second = GL_RED;
        break;
    case 3:
        formats.first = GL_RGB8;
        formats.second = GL_RGB;
        break;
    case 4:
        formats.first = GL_RGBA8;
        formats.second = GL_RGBA;
        break;
    default:
        break;
    }
    return formats;
}

OGLTexture::~OGLTexture()
{
    glDeleteTextures(1, &id);
}

bool OGLTexture::createTexture()
{
    std::pair<GLenum, GLenum> formats;
    GLint i = 0;            
    if (_imgs.empty() && _imgs[0]->empty())
        return (Created = false);
    switch (_type)
    {
        case Texture_Types::Skybox:
            glCreateTextures(GL_TEXTURE_CUBE_MAP_ARRAY, 1, &id);
            glTextureStorage3D(id, 1, GL_RGB8, _imgs[0]->w, _imgs[0]->h, 6);
            for (auto& img:_imgs)
                if (!img->empty())
                {
                    formats = Format(img->nrComponents);
                    glTextureSubImage3D(id, 0, 0, 0, i, img->w, img->h, 1, formats.second, GL_UNSIGNED_BYTE, img->_mdata);
                    i++;
                }
                else
                    return (Created = false);
            glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureParameteri(id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        break;
        case Texture_Types::Diffuse:
        case Texture_Types::Normal:
        case Texture_Types::Specular:
        case Texture_Types::Emissive:
        case Texture_Types::Height:
        case Texture_Types::Metallic_roughness:
        case Texture_Types::Ambient_occlusion:
            glCreateTextures(GL_TEXTURE_2D, 1, &id);
            glGenerateTextureMipmap(id);

            formats = Format(_imgs[0]->nrComponents);
            glTextureStorage2D(id, 1, formats.first, _imgs[0]->w, _imgs[0]->h);
            glTextureSubImage2D(id, 0, 0, 0, _imgs[0]->w, _imgs[0]->h, formats.second, GL_UNSIGNED_BYTE, _imgs[0]->_mdata);

            glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        break;
        default:
        break;
    }
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return (Created = true);
}

OGLMesh::~OGLMesh()
{
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void OGLMesh::setupMesh()
{
    //Culcs bytes layouts
    std::array<std::size_t, 5> Biases;
    std::size_t sum = 0;
    for (size_t i = 0; i < Biases.size(); ++i)
    {
        Biases[i] = sizeof(float) * sum;
        if (vertices->Has(static_cast<Vertexes::PointTypes>(i)))
            sum += vertices->GetSize();
    }
    std::size_t Bytes = sizeof(float) * sum;
    std::size_t BytesPerVector = sizeof(float) * BaseMesh::CardCoordsPerPoint;
    std::size_t BytesPerOneTypeVector = sizeof(float) * vertices->GetSize();
    const std::array<std::size_t, 5> points({ BaseMesh::CardCoordsPerPoint, BaseMesh::CardCoordsPerPoint, BaseMesh::CardCoordsPerTextPoint,BaseMesh::CardCoordsPerPoint,BaseMesh::CardCoordsPerPoint });
    // create buffers/arrays
    glCreateBuffers(1, &EBO);
    glCreateBuffers(1, &VBO);
    glCreateVertexArrays(1, &VAO);
    // load indices buffer
    glNamedBufferStorage(EBO, sizeof(std::uint32_t) * vertices->GetIndices().size(), vertices->GetIndices().data(), GL_DYNAMIC_STORAGE_BIT);
    //Allocate vertex buffer memory
    glNamedBufferStorage(VBO, Bytes, nullptr, GL_DYNAMIC_STORAGE_BIT);
    //Connect vertex and indices buffer
    glVertexArrayElementBuffer(VAO, EBO);
    //pos - 3,nor - 3, tex - 2, tan - 3, btan - 3 = 14 

    //glNamedBufferData(VBO[0], sizeof(GLfloat) * (vertices.Positions.size() + vertices.Normals.size() + vertices.TexCoords.size() + vertices.Tangents.size() + vertices.Bitangents.size()), NULL, GL_STATIC_DRAW);
    
    for (size_t i = 0; i < Biases.size(); ++i)
        if (vertices->Has(static_cast<Vertexes::PointTypes>(i)))
        {
            //Load vectors of vertices
            glNamedBufferSubData(VBO, Biases[i], BytesPerOneTypeVector, static_cast<const void*>(vertices->Get(static_cast<Vertexes::PointTypes>(i))));
            glEnableVertexArrayAttrib(VAO, i);
            //bind point with shader
            glVertexArrayAttribBinding(VAO, i, i);
            glVertexArrayVertexBuffer(VAO, i, VBO, Biases[i], BytesPerVector);
            glVertexArrayAttribFormat(VAO, i, points[i], GL_FLOAT, GL_FALSE, 0);
        }

    //Create mesh's textures
    for (auto& tex : textures)
        if (!tex->IsCreated())
            tex->createTexture();
}

void OGLMesh::Draw(const Shader* shader)
{
    for (std::size_t i = 0; i < textures.size(); ++i)
    {
        switch (textures[i]->GetType())
        {
        case Texture_Types::Diffuse:
            shader->setTexture("tex.diffuse", textures[i]->GetId(), static_cast<GLuint>(Texture_Types::Diffuse));
            break;
        case Texture_Types::Normal:
            shader->setTexture("tex.normal", textures[i]->GetId(), static_cast<GLuint>(Texture_Types::Normal));
            break;
        case Texture_Types::Specular:
            shader->setTexture("tex.specular", textures[i]->GetId(), static_cast<GLuint>(Texture_Types::Specular));
            break;
        case Texture_Types::Emissive:
            shader->setTexture("tex.emissive", textures[i]->GetId(), static_cast<GLuint>(Texture_Types::Emissive));
            break;
        case Texture_Types::Height:
            shader->setTexture("tex.height", textures[i]->GetId(), static_cast<GLuint>(Texture_Types::Height));
            break;
        case Texture_Types::Metallic_roughness:
            shader->setTexture("tex.metallic_roughness", textures[i]->GetId(), static_cast<GLuint>(Texture_Types::Metallic_roughness));
            break;
        case Texture_Types::Ambient_occlusion:
            shader->setTexture("tex.ao", textures[i]->GetId(), static_cast<GLuint>(Texture_Types::Ambient_occlusion));
            break;
        case Texture_Types::Skybox:
            shader->setTexture("skybox", textures[i]->GetId(), static_cast<GLuint>(Texture_Types::Skybox));
            break;
        }
    }
    //send material
    glUniform3f(glGetUniformLocation(shader->Program, "mat.ambient"), material->ambient.r, material->ambient.g, material->ambient.b);
    glUniform3f(glGetUniformLocation(shader->Program, "mat.diffuse"), material->diffuse.r, material->diffuse.g, material->diffuse.b);
    glUniform3f(glGetUniformLocation(shader->Program, "mat.specular"), material->specular.r, material->specular.g, material->specular.b);
    glUniform1f(glGetUniformLocation(shader->Program, "mat.shininess"), material->shininess);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, static_cast<GLsizei>(vertices->GetIndices().size()), GL_UNSIGNED_INT, nullptr, 1, 0, 0);
    //glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, indices.data());
    glBindVertexArray(0);
}