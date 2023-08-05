#include "stdafx.h"
#include "WireframeShader.h"

std::unique_ptr<ShaderParams> WireframeShader::CreateShaderParams()
{
	std::unique_ptr<ShaderParams> params = Shader::CreateShaderParams();

	params->m_isWireframe = true;

	return params;
}
