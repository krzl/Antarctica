#include "stdafx.h"
#include "AssetLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

template<>
std::shared_ptr<Texture> AssetLoader::Load(const std::string& path)
{
	int32_t        width, height, channels;
	const uint8_t* textureData = stbi_load(path.c_str(), &width, &height, &channels, 4);

	auto texture = std::make_shared<Texture>(textureData, width, height, channels, path);

	return texture;
}
