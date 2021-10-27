#include "STB_Loader.h"

void STB_Loader::LoadTexture(const void* path, std::size_t width, std::size_t height, bool FromProc)
{
    size = 1;
    if (FromProc)
    {
        if (height == 0)
        {
            data[0] = stbi_load_from_memory(static_cast<const unsigned char*>(path), static_cast<int>(width), &w[0], &h[0], &nrComponents[0], 0);
            //std::cout << stbi_failure_reason() << std::endl;
        }
        else
        {
            data[0] = new unsigned char[4 * width * height];
            std::copy_n(std::execution::par, static_cast<const unsigned char*>(path), 4 * width * height, data[0]);//need check out
            //data[0] = std::unique_ptr<unsigned char, decltype(&STB_Loader::DeleteTexture)>(static_cast<const unsigned char*>(path), &STB_Loader::DeleteTexture);
            w[0] = static_cast<int>(width);
            h[0] = static_cast<int>(height);
        }
    }
    else
        data[0] = stbi_load(static_cast<const char*>(path), &w[0], &h[0], &nrComponents[0], 0);

    if (data[0])
    {
        Loaded = true;
    }
    else
    {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        Loaded = false;
    }
}

void STB_Loader::LoadTexture(std::vector<std::string_view> paths)
{
    size = paths.size();
    for (std::size_t i = 0; i < paths.size(); ++i)
        data[i] = stbi_load(paths[i].data(), &w[i], &h[i], &nrComponents[i], 0);
}