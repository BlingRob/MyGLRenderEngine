#include "Model.h"

void Model::Draw(std::shared_ptr <Shader> sh)
{
    if(!sh)
        mRootNode->Draw(shader);
    else
        mRootNode->Draw(sh);
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

std::shared_ptr<Node> Model::GetRoot() 
{
    return mRootNode;
}
