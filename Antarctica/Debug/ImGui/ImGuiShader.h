#pragma once

#include "Assets/Shader.h"

class ImGuiShader : public Shader
{
public:

	std::unique_ptr<ShaderParams> CreateShaderParams() override;
};
