#pragma once
#include "Node.h"
#include "Shader.h"
#include "Transformation.h"
#include "Entity.h"
#include <memory>
#include <functional>

class Model:public Entity, public Transformation_interface
{
public:
    /*   Methods   */
    void Draw(const Shader*);
    void Draw();
    void SetRoot(std::shared_ptr<Node> root);
    std::shared_ptr<Node> GetRoot();

    void SetShader(std::shared_ptr <Shader> sh);
    const std::shared_ptr <Shader> GetShader();

    Model(std::function<void()> ResourceDeleter = []() {});
    Model(const Model&);
    Model(Model&&);
    Model& operator= (const Model&);
    Model&& operator=(Model&&);

    void SetTransform(const std::shared_ptr <glm::mat4>);
	void SetTransform(const glm::mat4&);
	glm::mat4 GetTransform() const;

	void Translate(const glm::vec3& trans);
	void Rotate(float alph, const glm::vec3& axes);
	void Scale(const glm::vec3& coefs);

private:
    std::function<void()> DeleterAssimpScene;
    /*  Model's data  */
    std::shared_ptr<Node>   mRootNode;
    std::shared_ptr<Shader> shader;
    void UpdateNodeTransformatins();
    Transformation tr;
};