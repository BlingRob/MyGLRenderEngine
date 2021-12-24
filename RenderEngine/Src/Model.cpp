#include "../Headers/Model.h"

void Model::Draw(const Shader* sh)
{
    mRootNode->Draw(sh ? sh : shader.get());
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

Model::Model(std::function<void()> del):DeleterAssimpScene(del)
{
}
Model::Model(const Model& mod) 
{
    mRootNode = mod.mRootNode;
    shader = mod.shader;
    tr = mod.tr;
    DeleterAssimpScene = mod.DeleterAssimpScene;
}

std::shared_ptr<Node> Model::GetRoot() 
{
    return mRootNode;
}
