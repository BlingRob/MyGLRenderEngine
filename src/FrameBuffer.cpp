#include "FrameBuffer.h"
FrameBuffer::FrameBuffer(GLuint weight, GLuint height)
{
    _mWeight = weight;
    _mHeight = height;
    textureID = Render = GL_NONE;
    glCreateFramebuffers(1, &FBO);
    shader = std::make_unique<Shader>("..\\Shaders\\PostEffects.vert", "..\\Shaders\\PostEffects.frag");

    glCreateBuffers(1, &EBO);
    glNamedBufferStorage(EBO, sizeof(Indices), Indices, 0);

    glCreateBuffers(1, &VBO);
    glNamedBufferStorage(VBO, sizeof(VerticesOfQuad), VerticesOfQuad, 0);

    glCreateVertexArrays(1, &VAO);
    glVertexArrayElementBuffer(VAO, EBO);

    glEnableVertexArrayAttrib(VAO, 0);
    glVertexArrayAttribBinding(VAO, 0, 0);
    glVertexArrayAttribFormat(VAO, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 2 * sizeof(GLfloat));
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &FBO);
    glDeleteRenderbuffers(1, &Render);
    glDeleteTextures(1, &textureID);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

std::tuple<GLenum, GLenum, GLenum> FrameBuffer::GetGlBufferType(BufferType BuffType)
{
    std::tuple<GLenum, GLenum, GLenum> attach;
    switch (BuffType)
    {
    case BufferType::Stencil:
        std::get<0>(attach) = GL_STENCIL_ATTACHMENT;
        std::get<1>(attach) = GL_R8;
        std::get<2>(attach) = GL_RED;
        break;
    case BufferType::Depth:
        std::get<0>(attach) = GL_DEPTH_ATTACHMENT;
        std::get<1>(attach) = GL_DEPTH_COMPONENT;
        std::get<2>(attach) = GL_RGB;
        break;
    case BufferType::Depth_Stencil:
        std::get<0>(attach) = GL_DEPTH_STENCIL_ATTACHMENT;
        std::get<1>(attach) = GL_DEPTH24_STENCIL8;
        std::get<2>(attach) = GL_UNSIGNED_INT_24_8;
        break;
    default:
    case BufferType::Color:
        std::get<0>(attach) = GL_COLOR_ATTACHMENT0;
        std::get<1>(attach) = GL_UNSIGNED_BYTE;
        std::get<2>(attach) = GL_RGB;
        break;
    }
    return attach;
}

void FrameBuffer::AddFrameBuffer(BufferType BuffType)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, textureID);
    auto [Attachment, Format, Type] = GetGlBufferType(BuffType);
    //glTextureStorage2D(textureID, 1, Format, weight, height);
    glTexImage2D(GL_TEXTURE_2D, 0, Type, _mWeight, _mHeight, 0, Type, Format, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glNamedFramebufferTexture(FBO, Attachment, textureID, 0);
}

void FrameBuffer::AddRenderBuffer(BufferType BuffType)
{
    glCreateRenderbuffers(1, &Render);
    auto [Attachment, Format, Type] = GetGlBufferType(BuffType);
    glNamedRenderbufferStorage(Render, Format, _mWeight, _mHeight);
    glNamedFramebufferRenderbuffer(FBO, Attachment, GL_RENDERBUFFER, Render);
}

bool FrameBuffer::IsCorrect() 
{
    return glCheckNamedFramebufferStatus(FBO, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void FrameBuffer::AttachBuffer() 
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}
void FrameBuffer::DetachBuffer() 
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Draw(uint64_t weight, uint64_t height)
{
    glDisable(GL_DEPTH_TEST);
    shader->Use();
    glBindTextureUnit(0, textureID);
    glUniform1i(glGetUniformLocation(shader->Program, "scene"), 0);
    shader->setScal("wAspect", static_cast<float>(weight) / _mWeight);
    shader->setScal("hAspect", static_cast<float>(height) / _mHeight);

    glBindVertexArray(VAO);
    glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, 1, 0, 0);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}