#include "../Headers/Mesh.h"

BaseMesh::BaseMesh(std::unique_ptr<Vertexes> verts, std::unique_ptr<Material> mat, std::vector<std::shared_ptr<BaseTexture>>&& texes)
{
	vertices = std::move(verts);
	material = (mat)?std::move(mat):std::make_unique<Material>();
	/* Textures */
	textures = std::forward<std::vector<std::shared_ptr<BaseTexture>>>(texes);
}

void BaseMesh::SetMaterial(std::unique_ptr<Material> mat)
{ 
	material = std::move(mat);
}

void BaseMesh::SetVertexes(std::unique_ptr<Vertexes> verts)
{ 
	vertices = std::move(verts);
}

void BaseMesh::AddTexture(std::shared_ptr<BaseTexture> tex)
{
	textures.emplace_back(std::move(tex));
}