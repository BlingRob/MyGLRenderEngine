#pragma once
#include "Headers.h"
#include "Mesh.h"
#include "Shader.h"
#include "Transformation.h"
#include "Entity.h"

class Node:public Entity
{
	
	std::list<std::shared_ptr<Node>> Children;
	std::list <std::shared_ptr <Mesh>> Meshes;
public:
	using MIt = std::list <std::shared_ptr<Mesh>>::const_iterator;
	using NIt = std::list <std::shared_ptr<Node>>::const_iterator;
	Node();
	Node(const Node&);

	Transformation tr;

	void addMesh(std::shared_ptr<Mesh> mesh);
	void addChild(std::shared_ptr<Node> child);
	std::pair<MIt, MIt> GetMeshes();
	std::pair<NIt, NIt> GetChildren();
	void Draw(const Shader* sh);
};

