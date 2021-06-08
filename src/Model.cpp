#include "Model.h"

void Model::Draw(const Shader* sh)
{
    if(sh != nullptr)
        mRootNode->Draw(sh);
    else
        mRootNode->Draw(shader.get());
}

void Model::SetRoot(std::shared_ptr<Node> root)
{
    mRootNode = root;
}

void Model::SetShader(std::shared_ptr <Shader> sh) 
{
    shader = sh;
}

const std::shared_ptr <Shader> Model::GetShader() 
{
    return shader;
}

Model::Model() 
{
}
Model::Model(const Model& mod) 
{
    mRootNode = std::make_shared<Node>(*mod.mRootNode);
    shader = mod.shader;
    tr = mod.tr;
}

std::shared_ptr<Node> Model::GetRoot() 
{
    return mRootNode;
}
