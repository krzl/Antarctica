#pragma once

#include "Asset.h"
#include "Shader.h"
#include "Texture.h"

class Shader;

class Material : public Asset
{
public:

	explicit Material(const std::shared_ptr<Shader>& shader) :
		m_shader(shader) { }

	[[nodiscard]] const std::shared_ptr<Shader>& GetShader() const
	{
		return m_shader;
	}

	[[nodiscard]] float GetOrder() const
	{
		return m_order;
	}

	void SetOrder(const float order)
	{
		m_order = order;
	}

	void SetTexture(const std::string& name, std::shared_ptr<Texture> texture)
	{
		m_textures[name] = std::move(texture);
	}

	[[nodiscard]] const std::unordered_map<std::string, std::shared_ptr<Texture>>& GetTextures() const
	{
		return m_textures;
	}

protected:

	bool Load(const std::string& path) override { return true; }

private:

	std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
	std::shared_ptr<Shader>                                   m_shader;
	float                                                     m_order = 0;
};
