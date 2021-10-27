#include "Node.h"

void Node::addMesh(std::shared_ptr <Mesh> mesh)
{
	Meshes.push_back(mesh);
}
void Node::addChild(std::shared_ptr<Node> child)
{
	Children.push_back(child);
}

void Node::Draw(const Shader* sh)
{

	for (const auto& mesh : Meshes)
	{
		tr.SendToShader(*sh);

		mesh->Draw(sh);
	}
	for (const auto& chld : Children)
		chld->Draw(sh);
}

Node::Node() 
{
	tr.Set(std::make_shared<glm::mat4>(1.0f));
}

Node::Node(const Node& node) 
{
	Children = node.Children;
	Meshes = node.Meshes;
	tr = node.tr;
}
std::pair<Node::MIt, Node::MIt> Node::GetMeshes() 
{
	return std::make_pair(Meshes.cbegin(), Meshes.cend());
}

std::pair<Node::NIt, Node::NIt> Node::GetChildren() 
{
	return std::make_pair(Children.cbegin(), Children.cend());
}