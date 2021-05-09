#include "Shadow.h"

Shadow::Shadow():FrameBuffer(ShadowMapSize, ShadowMapSize)
{
    shader = std::make_shared<Shader>("../Shaders/DepthMap.vs", "../Shaders/DepthMap.frag","../Shaders/DepthMap.gs");
    glNamedFramebufferReadBuffer(FBO, GL_NONE);
    glNamedFramebufferDrawBuffer(FBO, GL_NONE);
    id = *CubeMapIndex.begin();
    CubeMapIndex.pop_front();
   LightPos = glm::vec4(0.0f);
}
Shadow::~Shadow()
{
    //glDeleteTextures(1, &_3dTexture);
    CubeMapIndex.insert(std::lower_bound(CubeMapIndex.begin(), CubeMapIndex.end(), id), id);
}

void Shadow::InitOffsetTex(std::size_t size, int64_t samplesU, int64_t samplesV)
{
    int64_t samples = samplesU * samplesV;
    int64_t bufSize = size * size * samples * 2;
    std::unique_ptr<float[]> data = std::make_unique<float[]>(bufSize);

    std::random_device rd{};
    std::mt19937 gen{ rd() };
    std::uniform_real_distribution<> d{ -0.5,0.5 };

    std::size_t x1, y1, x2, y2;

    for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t j = 0; j < size; ++j) {
            for (std::size_t k = 0; k < samples; k += 2) {

                x1 = k % (samplesU);
                y1 = (samples - 1 - k) / samplesU;
                x2 = (k + 1) % samplesU;
                y2 = (samples - 1 - k - 1) / samplesU;

                glm::vec4 v;
                // Center on grid and jitter 
                v.x = (x1 + 0.5f) + d(gen);
                v.y = (y1 + 0.5f) + d(gen);
                v.z = (x2 + 0.5f) + d(gen);
                v.w = (y2 + 0.5f) + d(gen);
                // Scale between 0 and 1 
                v.x /= samplesU;
                v.y /= samplesV;
                v.z /= samplesU;
                v.w /= samplesV;
                // Warp to disk 
                std::size_t cell = ((k / 2) * size * size + j *
                    size + i) * 4;
                data[cell + 0] = glm::sqrt(v.y) * glm::cos(glm::two_pi<float>
                    () * v.x);
                data[cell + 1] = glm::sqrt(v.y) * glm::sin(glm::two_pi<float>
                    () * v.x);
                data[cell + 2] = glm::sqrt(v.w) * glm::cos(glm::two_pi<float>
                    () * v.z);
                data[cell + 3] = glm::sqrt(v.w) * glm::sin(glm::two_pi<float>
                    () * v.z);
            }
        }
    }

    glCreateTextures(GL_TEXTURE_3D, 1, &_3dTexture);

    glTextureStorage3D(_3dTexture, 1, GL_RGB8, size, size, samples / 2);
    glTextureSubImage3D(_3dTexture, 0, 0, 0, 0, size, size, samples / 2, GL_RGBA, GL_FLOAT, data.get());
    glTextureParameteri(_3dTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(_3dTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    radius = 3.0f / ShadowMapSize;
    OffsetTexSize = glm::vec3(size, size, (samplesU * samplesV) / 2.0f);
}

void Shadow::SendToShader(const Shader& sh)
{
    glBindTextureUnit(10, CubeMapArrayID);
    glUniform1i(glGetUniformLocation(sh.Program, "shadowMaps"), 10);
    glBindTextureUnit(11, _3dTexture);
    glUniform1i(glGetUniformLocation(sh.Program, "OffsetTex"), 11);
    sh.setScal("Radius", radius);
    sh.setVec("OffsetTexSize", OffsetTexSize);
    sh.setScal("far_plane", Far_Plane);
    Cleared = false;
}

void Shadow::AddShadowMap()
{
    GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    FrameBuffer::AddFrameBuffer(BufferType::Depth);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTextureParameterfv(textureID, GL_TEXTURE_BORDER_COLOR, borderColor);

    glTextureParameteri(textureID, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTextureParameteri(textureID, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
}
void Shadow::AddCubeShadowMap()
{
    //for(uint16_t i = 0; i < 6; ++i)
    //    glTextureSubImage3D(Shadow::CubeMapArrayID, 0, 0, 0, static_cast<GLint>(i), ShadowMapSize, ShadowMapSize, id + 1, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glNamedFramebufferTexture(FBO, GL_DEPTH_ATTACHMENT, Shadow::CubeMapArrayID, 0);
    //glNamedFramebufferTextureLayer(FBO, GL_DEPTH_ATTACHMENT, Shadow::CubeMapArrayID,0,)
}

void Shadow::Draw(std::pair<const_model_iterator, const_model_iterator> models,glm::vec4 lightPos) 
{
    static GLfloat OldView[4];
    static std::shared_ptr<Shader> sh(GetShader());
    AttachBuffer();
    glGetFloatv(GL_VIEWPORT, OldView);
    glViewport(0, 0, ShadowMapSize, ShadowMapSize);
    if (!Cleared)
    {
        glClearDepth(1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        Cleared = true;
    }
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.5f, 10.0f);
    glCullFace(GL_FRONT);
    if (LightPos != lightPos)
    {
        LightPos = lightPos;
        ViewMatrix[0] = ShadowProj * glm::lookAt(glm::vec3(LightPos), glm::vec3(LightPos) + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        ViewMatrix[1] = ShadowProj * glm::lookAt(glm::vec3(LightPos), glm::vec3(LightPos) + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        ViewMatrix[2] = ShadowProj * glm::lookAt(glm::vec3(LightPos), glm::vec3(LightPos) + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        ViewMatrix[3] = ShadowProj * glm::lookAt(glm::vec3(LightPos), glm::vec3(LightPos) + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        ViewMatrix[4] = ShadowProj * glm::lookAt(glm::vec3(LightPos), glm::vec3(LightPos) + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        ViewMatrix[5] = ShadowProj * glm::lookAt(glm::vec3(LightPos), glm::vec3(LightPos) + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

        pViewMatrix[0] = &ViewMatrix[0][0][0];
        pViewMatrix[1] = &ViewMatrix[1][0][0];
        pViewMatrix[2] = &ViewMatrix[2][0][0];
        pViewMatrix[3] = &ViewMatrix[3][0][0];
        pViewMatrix[4] = &ViewMatrix[4][0][0];
        pViewMatrix[5] = &ViewMatrix[5][0][0];
    }
    sh->Use();
    sh->setMats("shadowMatrices", pViewMatrix[0], AmountMatrixes);
    sh->setVec("lightPos", LightPos);
    sh->setScal("far_plane", Far_Plane);
    sh->setScal("Index", id);
    for (auto& it = models.first; it != models.second; ++it)
        it->second->Draw(sh);
    DetachBuffer();
    glDisable(GL_POLYGON_OFFSET_FILL);
    glCullFace(GL_BACK);
    glViewport(0, 0, OldView[2], OldView[3]);
}