#pragma once
//Texture's loader
#include <memory>
#include <vector>
#include <string_view>
#include "stb/stb_image.h"

using PDelFun = void(*)(void*);

struct Image
{
	Image() {}
	Image(Image& img) noexcept = default;
	Image(Image&& img) noexcept :w(img.w), h(img.h), nrComponents(img.nrComponents) { std::swap(_mdata,img._mdata); }
	Image&& operator= (Image&& img) noexcept
	{
		w = img.w;
		h = img.h;
		nrComponents = img.nrComponents;
		std::swap(_mdata, img._mdata);
		std::swap(deleter, img.deleter);
	}
	Image(std::size_t width, std::size_t height, std::uint8_t channel = 4, unsigned char* data = nullptr, PDelFun del = [](void* ptr) {delete ptr; }) :
		//_mdata(!data ? new void[, deleter] : std::make_shared<void>(data, deleter)),
		deleter(del), w(width), h(height), nrComponents(channel)
		{
			if (data == nullptr)
				_mdata = new unsigned char[width * height * channel];
			else
				_mdata = data;
		};
	bool empty() noexcept
	{
		return !w && !h;
	}
	~Image() 
	{ 
		deleter(_mdata);
	}
	unsigned char* _mdata;
	int w, h, nrComponents;
	private:
		PDelFun deleter;
};

struct ImageLoader
{
	static std::shared_ptr<Image> LoadTexture(const char* path);
	static std::shared_ptr<Image> LoadTexture(const void* memoryPtr, int width);
	static std::vector<std::shared_ptr<Image>> LoadTexture(std::vector<std::string_view> paths);
		
private:
	static inline int w, h, chs;
	static inline PDelFun STB_deleter = [](void* ptr) {stbi_image_free(ptr); };
};

