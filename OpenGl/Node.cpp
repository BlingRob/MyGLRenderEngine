#include "Node.h"

void Node::SetName(const std::string& name) 
{
	this->name = name;
}
std::string Node::GetName() const
{
	return name;
}
void Node::addMesh(std::shared_ptr <Mesh> mesh)
{
	//this->mesh = mesh;
	Meshes.push_back(mesh);
}
void Node::addChild(std::shared_ptr<Node> child)
{
	Children.push_back(child);
}

void Node::Draw(std::shared_ptr <Shader> sh)
{
		try
		{
			for (const auto& mesh : Meshes)
			{
				static glm::mat3 NormalMat;

				sh->setMat("transform.model", ModelMatrix);

				//glUniformMatrix4fv(glGetUniformLocation(shaders->Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

				NormalMat = glm::mat3(transpose(inverse(ModelMatrix)));
				sh->setMat("NormalMatrix", NormalMat);

				sh->Use();
				mesh->Draw(sh.get());
			}
		}
		catch (std::string str)
		{
			std::cerr << str << std::endl;
		}
		for (const auto& chld : Children)
			chld->Draw(sh);
}