#pragma once

#include "Headers.h"
#include "Node.h"
#include "Shader.h"
#include "Transformation.h"
#include "Entity.h"

class Model:public Entity
{
public:
    /*  Методы   */
    void Draw(std::shared_ptr <Shader>);
    void SetRoot(std::shared_ptr<Node> root);
    std::shared_ptr<Node> GetRoot();

    void SetShader(std::shared_ptr <Shader> sh);
    const std::shared_ptr <Shader> GetShader();

    Model();
private:
    /*  Данные модели  */
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::shared_ptr<Node> mRootNode;
    std::shared_ptr <Shader> shader;
    Transformation tr;
};