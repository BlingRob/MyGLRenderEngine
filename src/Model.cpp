#include "Model.h"

void Model::Draw()
{
    mRootNode->Draw(shader);
}

void Model::SetRoot(std::shared_ptr<Node> root)
{
    mRootNode = root;
}

std::string Model::GetName() const
{
    return name;
}
void Model::SetName(std::string name)
{
    this->name = name;
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
    ModelMatrix = glm::mat4(1.0f);
}



