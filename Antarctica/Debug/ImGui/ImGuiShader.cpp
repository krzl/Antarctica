#include "stdafx.h"
#include "ImGuiShader.h"

std::unique_ptr<ShaderParams> ImGuiShader::GetShaderParams()
{
	std::unique_ptr<ShaderParams> params = Shader::GetShaderParams();

	params->m_depthTestEnabled = false;
	params->m_blendingEnabled  = true;
	params->m_isDoubleSided    = true;

	return params;
}
