#include "stdafx.h"
#include "AssetLoader.h"

template<>
std::shared_ptr<ComputeShader> AssetLoader::Load(const std::string& path)
{
	return std::make_shared<ComputeShader>(path);
}
