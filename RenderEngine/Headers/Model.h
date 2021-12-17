#pragma once
#include "Node.h"
#include "Shader.h"
#include "Transformation.h"
#include "Entity.h"
#include <memory>
#include <functional>

class Model:public Entity
{
public:
    /*   Methods   */
    void Draw(const Shader*);
    void SetRoot(std::shared_ptr<Node> root);
    std::shared_ptr<Node> GetRoot();

    void SetShader(std::shared_ptr <Shader> sh);
    const std::shared_ptr <Shader> GetShader();

    Model(std::function<void()> ResourceDeleter = []() {});
    Model(const Model&);
private:
    std::function<void()> DeleterAssimpScene;
    /*  Model's data  */
    std::shared_ptr<Node>   mRootNode;
    std::shared_ptr<Shader> shader;
    Transformation tr;
};