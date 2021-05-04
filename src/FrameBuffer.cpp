#include "FrameBuffer.h"
FrameBuffer::FrameBuffer(GLuint weight, GLuint height)
{
    _mWeight = weight;
    _mHeight = height;
    textureID = Render = GL_NONE;
    glCreateFramebuffers(1, &FBO);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &FBO);
    glDeleteRenderbuffers(1, &Render);
    glDeleteTextures(1, &textureID);
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
        std::get<1>(attach) = GL_FLOAT;
        std::get<2>(attach) = GL_DEPTH_COMPONENT;
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
void FrameBuffer::SendToShader(std::shared_ptr<Shader> sh,std::string_view NameUniform)
{
    //GLint ind = glGetUniformLocation(sh->Program, "shadowMap");
    glBindTextureUnit(10, FrameBuffer::textureID);
    glUniform1i(glGetUniformLocation(sh->Program, NameUniform.data()), 10);
}

std::shared_ptr<Shader> FrameBuffer::GetShader()
{
    return shader;
}

PostProcessBuffer::PostProcessBuffer(GLuint weight, GLuint height):FrameBuffer(weight, height)
{
    shader = std::make_shared<Shader>("../Shaders/PostEffects.vert", "../Shaders/PostEffects.frag");

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
PostProcessBuffer::~PostProcessBuffer()
{
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void PostProcessBuffer::Draw(uint64_t weight, uint64_t height)
{
    glDisable(GL_DEPTH_TEST);
    shader->Use();
    SendToShader(shader, "scene");
    shader->setScal("wAspect", static_cast<float>(weight) / FrameBuffer::_mWeight);
    shader->setScal("hAspect", static_cast<float>(height) / FrameBuffer::_mHeight);
    if(invertion)
        shader->setSubroutine(GL_FRAGMENT_SHADER, "Invertion");
    else if (convolution && Core)
    {
        shader->setSubroutine(GL_FRAGMENT_SHADER, "Convolution");
        shader->setMat("kernel", *Core);
    }
    else
        shader->setSubroutine(GL_FRAGMENT_SHADER, "Default");

    glBindVertexArray(VAO);
    glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, 1, 0, 0);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

ShadowMapBuffer::ShadowMapBuffer(GLuint weight, GLuint height):FrameBuffer(weight, height)
{
    shader = std::make_shared<Shader>("../Shaders/DepthMap.vs", "../Shaders/DepthMap.frag");
    glNamedFramebufferReadBuffer(FBO, GL_NONE);
    glNamedFramebufferDrawBuffer(FBO, GL_NONE);
}
void ShadowMapBuffer::AddFrameBuffer(BufferType BuffType) 
{
    GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    FrameBuffer::AddFrameBuffer(BuffType);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTextureParameterfv(textureID, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTextureParameteri(textureID, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTextureParameteri(textureID, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
}
