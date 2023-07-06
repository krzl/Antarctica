#include "stdafx.h"
#include "Assets/ComputeShader.h"
#include "Assets/Shader.h"

//return true immediately, asset load errors will be reported during shader compilation

bool ComputeShader::Load(const std::string& path)
{
	m_path = path;
	return true;
}

bool Shader::Load(const std::string& path)
{
	m_path = path;
	return true;
}
