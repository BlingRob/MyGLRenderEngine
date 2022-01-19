#include "../Headers/Model.h"

Model::Model(std::function<void()> del):DeleterAssimpScene(del)
{
}

Model::Model(const Model& mod)
{
    SetRoot(mod.mRootNode);
    tr = mod.tr;
    shader = mod.shader;
    DeleterAssimpScene = mod.DeleterAssimpScene;
}

Model::Model(Model&& mod)
{
    mRootNode = std::move(mod.mRootNode);
    tr = mod.tr;
    shader = std::move(mod.shader);
    DeleterAssimpScene = std::move(mod.DeleterAssimpScene);
}

Model& Model::operator= (const Model& right) 
{
    SetRoot(right.mRootNode);
    tr = right.tr;
    shader = right.shader;
    DeleterAssimpScene = right.DeleterAssimpScene;
    return *this;
}

Model&& Model::operator=(Model&& right) 
{
    mRootNode = std::move(right.mRootNode);
    tr = right.tr;
    shader = std::move(right.shader);
    DeleterAssimpScene = std::move(right.DeleterAssimpScene);
    return std::move(*this);
}

void Model::Draw(const Shader* sh)
{
    mRootNode->Draw(sh ? sh : shader.get());
}

void Model::Draw()
{
    mRootNode->Draw(shader.get());
}

void Model::SetRoot(std::shared_ptr<Node> root)
{
    mRootNode = std::make_shared<Node>(*root);
}

void Model::SetShader(std::shared_ptr<Shader> sh)
{
    shader = sh;
}

const std::shared_ptr <Shader> Model::GetShader()
{
    return shader;
}

std::shared_ptr<Node> Model::GetRoot()
{
    return mRootNode;
}

void Model::SetTransform(const std::shared_ptr <glm::mat4> mat)
{
    tr.SetTransform(mat);
    mRootNode->SetTransform(mRootNode->GetTransform() * GetTransform());
}

void Model::SetTransform(const glm::mat4& mat)
{
    tr.SetTransform(mat);
    mRootNode->SetTransform(mRootNode->GetTransform() * GetTransform());
}

glm::mat4 Model::GetTransform() const
{
    return mRootNode->GetTransform();
}

void Model::Translate(const glm::vec3& trans)
{
    tr.Translate(trans);
    mRootNode->Translate(trans);
}

void Model::Rotate(float alph, const glm::vec3& axes)
{
    tr.Rotate(alph,axes);
    mRootNode->Rotate(alph,axes);
}

void Model::Scale(const glm::vec3& coefs)
{
    tr.Scale(coefs);
    mRootNode->Scale(coefs);
}
