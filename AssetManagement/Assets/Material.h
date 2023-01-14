#pragma once

#include <Shaders/MaterialObject.h>
#include "Shader.h"
#include "../Asset.h"

class Shader;

class Material : public Asset
{
public:

	explicit Material(std::shared_ptr<Shader>& shader) :
		m_materialObject(Renderer::MaterialObject(shader->GetShaderObject())),
		m_shader(shader) { }

	[[nodiscard]] const std::shared_ptr<Shader>& GetShader() const
	{
		return m_shader;
	}

	[[nodiscard]] const Renderer::MaterialObject& GetMaterialObject() const
	{
		return m_materialObject;
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
		m_materialObject.m_textures[name] = std::move(texture);
	}

private:

	Renderer::MaterialObject m_materialObject;
	std::shared_ptr<Shader>  m_shader;
	float                    m_order = 0;
};
