#pragma once

#include "../AssetManagement/Asset.h"

#include "Shaders/ShaderObject.h"

namespace Renderer
{
	class MaterialObject;
}

class Shader : public Asset
{
	friend class Renderer::MaterialObject;

public:

	explicit Shader(const Renderer::ShaderObject shaderObject) :
		m_shaderObject(shaderObject)
	{
	}

	[[nodiscard]] const Renderer::ShaderObject& GetShaderObject() const
	{
		return m_shaderObject;
	}

	[[nodiscard]] Renderer::ShaderObject& GetShaderObject()
	{
		return m_shaderObject;
	}
	
private:
	
	Renderer::ShaderObject m_shaderObject;
};
