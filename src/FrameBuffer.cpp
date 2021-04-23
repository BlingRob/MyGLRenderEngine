#include "FrameBuffer.h"
FrameBuffer::FrameBuffer()
{
    textureID = Render = GL_NONE;
    glCreateFramebuffers(1, &buff);
    shader = std::make_unique<Shader>("..\\Shaders\\PostEffects.vert", "..\\Shaders\\PostEffects.frag");

    glCreateBuffers(1, &EBO);
    glNamedBufferStorage(EBO, sizeof(GLuint) * 6, Indices, GL_MAP_READ_BIT);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glCreateBuffers(1, &VBO);
    glNamedBufferStorage(VBO, sizeof(GLfloat) * 12, VerticesOfQuad, GL_MAP_READ_BIT);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexArrayAttribBinding(VAO, 0, 0);
    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(GLfloat) * 3);
    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glBindVertexArray(GL_NONE);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &buff);
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

void FrameBuffer::AddFrameBuffer(BufferType BuffType, GLuint weight, GLuint height)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, textureID);
    auto [Attachment, Format, Type] = GetGlBufferType(BuffType);

    glTexImage2D(GL_TEXTURE_2D, 0, Type, weight, height, 0, Type, Format, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glNamedFramebufferTexture(buff, Attachment, textureID, 0);
}

void FrameBuffer::AddRanderBuffer(BufferType BuffType, GLuint weight, GLuint height)
{
    glCreateRenderbuffers(1, &Render);
    auto [Attachment, Format, Type] = GetGlBufferType(BuffType);
    glNamedRenderbufferStorage(Render, Format, weight, height);
    glNamedFramebufferRenderbuffer(buff, Attachment, GL_RENDERBUFFER, Render);
}

bool FrameBuffer::IsCorrect() 
{
    return glCheckNamedFramebufferStatus(buff, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void FrameBuffer::AttachBuffer() 
{
    glBindFramebuffer(GL_FRAMEBUFFER, buff);
}
void FrameBuffer::DetachBuffer() 
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Draw() 
{
    shader->Use();
    glBindTextureUnit(0, textureID);
    glUniform1i(glGetUniformLocation(shader->Program, "scene"), 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, Indices);
    glBindVertexArray(0);
}