#pragma once

#include "../Asset.h"

#include <Objects/TextureObject.h>

class Texture : public Asset
{
public:

	Texture()
	{
	}

	Texture(const uint8_t* data, const int32_t width, const int32_t height, const int32_t channels)
	{
		m_textureObject.m_data     = data;
		m_textureObject.m_width    = width;
		m_textureObject.m_height   = height;
		m_textureObject.m_channels = channels;
	}

	[[nodiscard]] Renderer::TextureObject& GetTextureObject()
	{
		return m_textureObject;
	}


	[[nodiscard]] const Renderer::TextureObject& GetTextureObject() const
	{
		return m_textureObject;
	}

private:

	Renderer::TextureObject m_textureObject;
};
