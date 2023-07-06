#include "stdafx.h"
#include "Assets/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

bool Texture::Load(const std::string& path)
{
	m_data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 4);

	m_path = std::move(path);

	return m_data != nullptr;
}
