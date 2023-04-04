#include "stdafx.h"
#include "ShaderDescriptor.h"

namespace Renderer
{
	void ShaderDescriptor::AddTextureDescriptor(TextureDescriptor&& descriptor)
	{
		m_textures.push_back(std::move(descriptor));
	}

	void ShaderDescriptor::AddVariableDescriptor(VariableDescriptor&& descriptor)
	{
		m_variables.push_back(std::move(descriptor));
	}

	void ShaderDescriptor::AddBufferDescriptor(BufferDescriptor&& descriptor)
	{
		m_buffers.push_back(std::move(descriptor));
	}

	bool ShaderDescriptor::ContainsBuffer(const std::string& name) const
	{
		for (const auto& [bufferName, bufferSize, defaultValue] : m_buffers)
		{
			if (bufferName == name)
			{
				return true;
			}
		}
		return false;
	}

	bool ShaderDescriptor::ContainsTextureId(const uint32_t id) const
	{
		for (const auto& [name, textureId] : m_textures)
		{
			if (textureId == id)
			{
				return true;
			}
		}
		return false;
	}

	void ShaderDescriptor::Clear()
	{
		m_textures.clear();
	}

	ShaderDescriptor::~ShaderDescriptor()
	{
		// ReSharper disable once CppUseStructuredBinding
		for (const BufferDescriptor& bufferInfo : m_buffers)
		{
			delete[] bufferInfo.m_defaultValue;
		}
	}
}
