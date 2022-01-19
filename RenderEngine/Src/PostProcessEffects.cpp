#include "../Headers/PostProcessEffects.h"

ProcessEffect::ProcessEffect(uint32_t weight, uint32_t height):FrameBuffer(weight, height)
{
	_pShader = ShadersBank::GetShader("PostEffect");
    std::vector<uint32_t> CopyInds = std::vector<uint32_t>(Indices);
	_pMesh = std::make_unique<Mesh>(std::make_unique<Vertexes>(12, nullptr, std::move(CopyInds), VerticesOfQuad));
    Core = std::make_shared<glm::mat3>(1.0f);
    _pMesh->setupMesh();
    AddFrameBuffer(BufferType::Color);
    AddRenderBuffer(BufferType::Depth_Stencil);
    if (!IsCorrect())
        std::cerr << "Creating addition buffer is failed";
}

void ProcessEffect::Draw(uint64_t CurWeight, uint64_t CurHeight) 
{
    glDisable(GL_DEPTH_TEST);
    _pShader->Use();
    SendToShader(*_pShader, "scene", PointBindTexture);
    _pShader->setScal("wAspect", static_cast<float>(CurWeight) / FrameBuffer::_mWeight);
    _pShader->setScal("hAspect", static_cast<float>(CurHeight) / FrameBuffer::_mHeight);
    if (invertion)
        _pShader->setSubroutine("Invertion", GL_FRAGMENT_SHADER);
    else if (convolution && Core)
    {
        _pShader->setSubroutine("Convolution", GL_FRAGMENT_SHADER);
        _pShader->setMat("kernel", *Core);
    }
    else
        _pShader->setSubroutine("Default", GL_FRAGMENT_SHADER);
    _pMesh->Draw(_pShader.get());
    glEnable(GL_DEPTH_TEST);
}

ProcessEffect::~ProcessEffect()
{

}