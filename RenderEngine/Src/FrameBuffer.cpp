#include "../Headers/FrameBuffer.h"
FrameBuffer::FrameBuffer(GLuint weight, GLuint height)
{
    _mWeight = weight;
    _mHeight = height;
    textureID = Render = GL_NONE;
    RenderInclude = TextureInclude = false;
    OldFBO = GL_NONE;
    glCreateFramebuffers(1, &FBO);
}

FrameBuffer::FrameBuffer(FrameBuffer&& fr) noexcept
{
    _mWeight = fr._mWeight;
    _mHeight = fr._mHeight;
    RenderInclude = fr.RenderInclude;
    TextureInclude = fr.TextureInclude;
    std::swap(textureID, fr.textureID);
    std::swap(Render, fr.Render);
    std::swap(OldFBO, fr.OldFBO);
    std::swap(FBO, fr.FBO);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &FBO);

    if(RenderInclude)
       glDeleteRenderbuffers(1, &Render);
    if(TextureInclude)
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
    TextureInclude = true;
}

void FrameBuffer::AddRenderBuffer(BufferType BuffType)
{
    glCreateRenderbuffers(1, &Render);
    auto [Attachment, Format, Type] = GetGlBufferType(BuffType);
    glNamedRenderbufferStorage(Render, Format, _mWeight, _mHeight);
    glNamedFramebufferRenderbuffer(FBO, Attachment, GL_RENDERBUFFER, Render);
    RenderInclude = true;
}

bool FrameBuffer::IsCorrect() 
{
    return glCheckNamedFramebufferStatus(FBO, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void FrameBuffer::AttachBuffer() 
{
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &OldFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}
void FrameBuffer::DetachBuffer() 
{
    glBindFramebuffer(GL_FRAMEBUFFER, OldFBO);
}
void FrameBuffer::SendToShader(const Shader& sh,std::string_view NameUniform, std::uint16_t BindIndex)
{
    sh.setTexture(NameUniform, FrameBuffer::textureID, BindIndex);
}