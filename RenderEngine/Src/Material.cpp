#include "../Headers/Material.h"

Vertexes::Vertexes(std::size_t size, const float* positions, std::vector<std::uint32_t>&& inds, const float* normals, const float* textureCoords, const float* tangent, const float* bitangent)
{
	if (size == 0)
		return;
	VectorsSize = size;

	if (positions)
	{
		vectors[PointTypes::positions] = positions;
		HasPointType.set(PointTypes::positions);
	}

	if (!inds.empty()) 
	{
		indices = std::forward<std::vector<std::uint32_t>>(inds);
	}

	if (normals) 
	{
		vectors[PointTypes::normals] = normals;
		HasPointType.set(PointTypes::normals);
	}

	if (textureCoords)
	{
		vectors[PointTypes::texture] = textureCoords;
		HasPointType.set(PointTypes::texture);
	}

	if (tangent)
	{
		vectors[PointTypes::tangent] = tangent;
		HasPointType.set(PointTypes::tangent);
	}

	if (bitangent)
	{
		vectors[PointTypes::bitangent] = bitangent;
		HasPointType.set(PointTypes::bitangent);
	}
}

bool Vertexes::Has(PointTypes type)
{
	return HasPointType[type];
}

const float* Vertexes::Get(PointTypes type)
{ 
	return vectors[type];
}

std::vector<std::uint32_t>& Vertexes::GetIndices()
{
	return indices;
}

void Vertexes::AddIndices(std::vector<std::uint32_t>&& inds)
{
	indices = std::forward<std::vector<std::uint32_t>>(inds);
}

Vertexes::Vertexes(Vertexes&& vecs) 
{
	vectors = std::forward<std::array<const float*, 5>>(vecs.vectors);
	HasPointType= std::forward<std::bitset<5>>(vecs.HasPointType);
	indices = std::forward<std::vector<std::uint32_t>>(vecs.indices);
}

Vertexes&& Vertexes::operator=(Vertexes&& vecs)
{
	vectors = std::forward<std::array<const float*, 5>>(vecs.vectors);
	HasPointType = std::forward<std::bitset<5>>(vecs.HasPointType);
	indices = std::forward<std::vector<std::uint32_t>>(vecs.indices);

	return std::forward<Vertexes>(*this);
}

std::size_t Vertexes::GetSize()
{
	return VectorsSize;
}

BaseTexture::BaseTexture(std::string& name, std::string& path, Texture_Types type, std::vector<std::shared_ptr<Image>> images) 
{
	SetName(name);
	_path = path;
	_type = type;
	_imgs = images;
}

bool BaseTexture::IsCreated()
{
	return Created;
}

unsigned int BaseTexture::GetId() 
{ 
	return id;
}

std::string BaseTexture::GetPath()
{ 
	return _path;
}

Texture_Types BaseTexture::GetType()
{ 
	return _type;
}

std::vector<std::shared_ptr<Image>> BaseTexture::GetImages()
{ 
	return _imgs;
}

void BaseTexture::AddImage(std::shared_ptr<Image> img) 
{
	_imgs.emplace_back(std::move(img));
}