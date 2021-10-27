#pragma once
#include "Headers.h"
//Texture's loader
#include "stb/stb_image.h"


struct STB_Loader
{
		static const std::size_t MaxSize = 6;
		STB_Loader():size(0),Loaded(false) {}
		STB_Loader(const void* path, std::size_t width, std::size_t height, bool FromProc) { LoadTexture(path,width,height,FromProc); }
		STB_Loader(std::vector<std::string_view> paths) { LoadTexture(paths); }
		//STB_Loader(unsigned char* data, int w, int h, int nrComponents) : data(data), w(w), h(h), nrComponents(nrComponents) {}

		void LoadTexture(const void* path, std::size_t width, std::size_t height, bool FromProc);
		void LoadTexture(std::vector<std::string_view> paths);

		~STB_Loader() 
		{
			for(std::uint16_t i = 0;i < size;++i)
			if (data[i] != nullptr) 
				stbi_image_free(data[i]);
		}
		//~STB_Loader() {};
		unsigned char* data[MaxSize];
		//std::unique_ptr<std::unique_ptr<unsigned char, decltype(&DeleteTexture)>[]> data;
		int w[MaxSize], h[MaxSize], nrComponents[MaxSize];
		std::size_t size;
		bool Loaded;
};

