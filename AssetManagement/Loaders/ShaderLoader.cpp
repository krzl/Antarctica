#include "stdafx.h"
#include "AssetLoader.h"

template<>
std::shared_ptr<Shader> AssetLoader::Load(const std::string& path)
{
	return std::make_shared<Shader>(Renderer::ShaderObject(path));
}