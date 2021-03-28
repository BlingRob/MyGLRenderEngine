#pragma once
#include "Headers.h"
#include "Mesh.h"
#include "Shader.h"

class Node
{
	std::string name;
	std::list<std::shared_ptr<Node>> Children;
	std::list < std::shared_ptr <Mesh>> Meshes;
public:

	glm::mat4 ModelMatrix;

	std::string GetName() const;
	void SetName(const std::string& name);

	void addMesh(std::shared_ptr<Mesh> mesh);
	void addChild(std::shared_ptr<Node> child);
	void Draw(std::shared_ptr <Shader> sh);
};

