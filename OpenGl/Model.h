#pragma once

#include "Headers.h"
#include "Node.h"
#include "Shader.h"

class Model
{
public:
    /*  Методы   */
    void Draw();
    void SetRoot(std::shared_ptr<Node> root);

    std::string GetName() const;
    void SetName(std::string name);
    void SetShader(std::shared_ptr <Shader> sh);

    Model();
private:
    /*  Данные модели  */
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::shared_ptr<Node> mRootNode;
    std::string name;
    std::shared_ptr <Shader> shader;
    glm::mat4 ModelMatrix;
};