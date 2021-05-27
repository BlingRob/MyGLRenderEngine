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
Model::Model(const Model& mod) 
{
    textures_loaded = mod.textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    mRootNode = std::make_shared<Node>(*mod.mRootNode);
    shader = mod.shader;
    tr = mod.tr;
}

std::shared_ptr<Node> Model::GetRoot() 
{
    return mRootNode;
}
