#pragma once

#include "Assets/Shader.h"

class WireframeShader : public Shader
{
public:

	WireframeShader() = default;

	explicit WireframeShader(const std::string& path)
	{
		m_path = path;
	}

	std::unique_ptr<ShaderParams> CreateShaderParams() override;
};
