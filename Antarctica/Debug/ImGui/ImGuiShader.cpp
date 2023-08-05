#include "stdafx.h"
#include "ImGuiShader.h"

std::unique_ptr<ShaderParams> ImGuiShader::CreateShaderParams()
{
	std::unique_ptr<ShaderParams> params = Shader::CreateShaderParams();

	params->m_depthTestEnabled = false;
	params->m_blendingEnabled  = true;
	params->m_isDoubleSided    = true;

	return params;
}
