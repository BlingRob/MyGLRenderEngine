#include "Node.h"

void Node::addMesh(std::shared_ptr <Mesh> mesh)
{
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
				tr.SendToShader(*sh);

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

Node::Node() 
{
	tr.Set(std::make_shared<glm::mat4>(1.0f));
}