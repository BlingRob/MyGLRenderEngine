#include "Shadow.h"

Shadow::Shadow():FrameBuffer(ShadowMapSize, ShadowMapSize)
{
    glNamedFramebufferReadBuffer(*FBO, GL_NONE);
    glNamedFramebufferDrawBuffer(*FBO, GL_NONE);
    LightVector = glm::vec4(0.0f);
}

Shadow::Shadow(const Shadow& shadow):FrameBuffer(dynamic_cast<const FrameBuffer&>(shadow))
{
    id = shadow.id;
}

Shadow::~Shadow()
{
}

PointShadow::PointShadow()
{
    if (!init) 
    {      
        shader = std::make_shared<Shader>("../Shaders/PointDepthMap.vs", "../Shaders/PointDepthMap.frag", "../Shaders/PointDepthMap.gs");
        glCreateTextures(GL_TEXTURE_CUBE_MAP_ARRAY, 1, &ShadowArrayID);

        glTextureStorage3D(ShadowArrayID, 1, GL_DEPTH_COMPONENT16, Shadow::ShadowMapSize, Shadow::ShadowMapSize, MAX_LIGHTS_ONE_TYPE * 6);
        glTextureParameteri(ShadowArrayID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(ShadowArrayID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(ShadowArrayID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTextureParameteri(ShadowArrayID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(ShadowArrayID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameterfv(ShadowArrayID, GL_TEXTURE_BORDER_COLOR, borderColor);

        init = true;
    }
    ShadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, Shadow::Far_Plane);
}

PointShadow::PointShadow(const PointShadow& ps) noexcept
{
    ShadowProj = ps.ShadowProj;
}

PointShadow::~PointShadow()
{
}

void Shadow::AddBuffer()
{

}
GLuint DirectionShadow::AddBuffer(GLuint ShadowId)
{
    id = ShadowId;
    glNamedFramebufferTextureLayer(*FBO, GL_DEPTH_ATTACHMENT, ShadowArrayID, 0, id);
    //glNamedFramebufferTexture(FBO, GL_DEPTH_ATTACHMENT, ShadowArrayID, 0);
    return *FBO;
}
GLuint PointShadow::AddBuffer(GLuint ShadowId)
{
    id = ShadowId;
    glNamedFramebufferTexture(*FBO, GL_DEPTH_ATTACHMENT, ShadowArrayID, 0);
    return *FBO;
}

void DirectionShadow::InitOffsetTex(std::size_t size, int64_t samplesU, int64_t samplesV)
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

    radius = 3 / 1024.;
    OffsetTexSize = glm::vec3(size, size, (samplesU * samplesV) / 2.0f);
}

void PointShadow::SendToShader(const Shader& sh)
{
    sh.setTexture(PointNameShadowUniformTexture, ShadowArrayID, PointShadowBind);
    sh.setScal("far_plane", Far_Plane);
    sh.setMat("lightProjection", ShadowProj);
}
void DirectionShadow::SendToShader(const Shader& sh)
{
    sh.setMat(_mStrNumLight, scale_bias_matrix * (*LightMat.Projection) * (*LightMat.View));
    sh.setTexture(SpotAndDirNameShadowUniformTexture, ShadowArrayID, SpotAndDirShadowBind);
    sh.setTexture("OffsetTex", _3dTexture, 12);
    sh.setScal("Radius", radius);
    sh.setVec("OffsetTexSize", OffsetTexSize);
}

DirectionShadow::DirectionShadow(const std::string& StrNumLight)
{
    if (!init) 
    {
        shader = std::make_shared<Shader>("../Shaders/DirectDepthMap.vs", "../Shaders/DirectDepthMap.frag");

        glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &ShadowArrayID);

        glTextureStorage3D(ShadowArrayID, 1, GL_DEPTH_COMPONENT16, Shadow::ShadowMapSize, Shadow::ShadowMapSize, 2 * MAX_LIGHTS_ONE_TYPE);
        glTextureParameteri(ShadowArrayID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(ShadowArrayID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTextureParameteri(ShadowArrayID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(ShadowArrayID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTextureParameterfv(ShadowArrayID, GL_TEXTURE_BORDER_COLOR, borderColor);

        //glTextureParameteri(ShadowArrayID, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        //glTextureParameteri(ShadowArrayID, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

        scale_bias_matrix = glm::mat4(0.5f);
        scale_bias_matrix[3] = glm::vec4(0.5f);
        scale_bias_matrix[3][3] = 1;

        InitOffsetTex(16, 8, 8);
        init = true;
    }

    _mStrNumLight = (StrNumLight + "ShadowMatrix");
    LightMat.Projection = std::make_shared<glm::mat4>(glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -30.0f, 30.0f));
    //LightMat.Projection = std::make_shared<glm::mat4>(glm::ortho(-static_cast<float>(ShadowMapSize)/2, static_cast<float>(ShadowMapSize) / 2, -static_cast<float>(ShadowMapSize) / 2, static_cast<float>(ShadowMapSize) / 2, 0.1f, Shadow::Far_Plane));
    //LightMat.Projection = std::make_shared<glm::mat4>(glm::ortho(-static_cast<float>(ShadowMapSize), static_cast<float>(ShadowMapSize), 0.1f, Shadow::Far_Plane));
}


void PointShadow::Draw(std::pair<const_model_iterator, const_model_iterator> models,glm::vec4 lightPos)
{
    static GLfloat OldView[4];
    AttachBuffer();
    glGetFloatv(GL_VIEWPORT, OldView);
    glViewport(0, 0, ShadowMapSize, ShadowMapSize);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.5f, 10.0f);
    //glCullFace(GL_FRONT);
    if (LightVector != lightPos)
    {
        LightVector = lightPos;
        ViewMatrix[0] = ShadowProj * glm::lookAt(glm::vec3(LightVector), glm::vec3(LightVector) + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        ViewMatrix[1] = ShadowProj * glm::lookAt(glm::vec3(LightVector), glm::vec3(LightVector) + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        ViewMatrix[2] = ShadowProj * glm::lookAt(glm::vec3(LightVector), glm::vec3(LightVector) + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        ViewMatrix[3] = ShadowProj * glm::lookAt(glm::vec3(LightVector), glm::vec3(LightVector) + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        ViewMatrix[4] = ShadowProj * glm::lookAt(glm::vec3(LightVector), glm::vec3(LightVector) + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        ViewMatrix[5] = ShadowProj * glm::lookAt(glm::vec3(LightVector), glm::vec3(LightVector) + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

        pViewMatrix[0] = &ViewMatrix[0][0][0];
        pViewMatrix[1] = &ViewMatrix[1][0][0];
        pViewMatrix[2] = &ViewMatrix[2][0][0];
        pViewMatrix[3] = &ViewMatrix[3][0][0];
        pViewMatrix[4] = &ViewMatrix[4][0][0];
        pViewMatrix[5] = &ViewMatrix[5][0][0];
    }
    shader->Use();
    shader->setMats("shadowMatrices", pViewMatrix[0], AmountMatrixes);
    shader->setVec("lightPos", LightVector);
    shader->setScal("far_plane", Far_Plane);
    shader->setScal("Index", id);
    for (auto& it = models.first; it != models.second; ++it)
        it->second->Draw(shader.get());
    DetachBuffer();
    glDisable(GL_POLYGON_OFFSET_FILL);
    //glCullFace(GL_BACK);
    glViewport(0, 0, OldView[2], OldView[3]);
}

void DirectionShadow::Draw(std::pair<const_model_iterator, const_model_iterator> Models, glm::vec4 direction)
{
    static GLfloat OldView[4];
    AttachBuffer();
    glGetFloatv(GL_VIEWPORT, OldView);
    glViewport(0, 0, ShadowMapSize, ShadowMapSize);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.5f, 10.0f);
    glCullFace(GL_FRONT);
    const glm::vec3 SceneCenter(0.0f,2.0f,0.0f);
    //LightMat.View = std::make_shared<glm::mat4>(glm::lookAt(SceneCenter,
   //     SceneCenter + glm::vec3(direction),
   //        glm::vec3(0.0f, 1.0f, 0.0f)));
   LightMat.View = std::make_shared<glm::mat4>(glm::lookAt(glm::vec3(0.0f),
        -glm::vec3(direction),
           glm::vec3(0.0f, -1.0f, 0.0f)));
    shader->Use();
    LightMat.SendToShader(*shader);
    //shader->setScal("Index", id);
    for (auto& it = Models.first; it != Models.second; ++it)
        it->second->Draw(shader.get());
    DetachBuffer();
    glDisable(GL_POLYGON_OFFSET_FILL);
    glCullFace(GL_BACK);
    glViewport(0, 0, OldView[2], OldView[3]);
}


DirectionShadow::DirectionShadow(const DirectionShadow& ds) noexcept
{
    LightMat = ds.LightMat;
    _3dTexture = ds._3dTexture;
    radius = ds.radius;
    OffsetTexSize = ds.OffsetTexSize;
    //StrShadowMatrix = ds.StrShadowMatrix;
}

DirectionShadow::~DirectionShadow() 
{
    glDeleteTextures(1, &_3dTexture);
}