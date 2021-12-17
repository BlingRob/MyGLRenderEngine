#include "../../Headers/Loaders/ImageLoader.h"

std::shared_ptr<Image> ImageLoader::LoadTexture(const char* path)
{
    return std::make_shared<Image>(w, h, chs, stbi_load(path, &w, &h, &chs, 0), STB_deleter );
}

std::shared_ptr<Image> ImageLoader::LoadTexture(const void* memoryPtr, int bytes)
{
    return std::make_shared<Image>(w, h, chs, stbi_load_from_memory(static_cast<const stbi_uc*>(memoryPtr), bytes, &w, &h, &chs, 0), STB_deleter);
}