#pragma once
#include "Headers.h"
#include "Mesh.h"
#include "Shader.h"
#include "Transformation.h"
#include "Entity.h"

class Node:public Entity
{
	std::list<std::shared_ptr<Node>> Children;
	std::list < std::shared_ptr <Mesh>> Meshes;
public:
	Node();
	Node(const Node&);

	Transformation tr;

	void addMesh(std::shared_ptr<Mesh> mesh);
	void addChild(std::shared_ptr<Node> child);
	void Draw(const Shader* sh);
};

