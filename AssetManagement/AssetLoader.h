#pragma once

#include "Assets/Mesh.h"
#include "Assets/Shader.h"

class AssetLoader
{
	friend class AssetManager;
	
	template<typename T>
	static std::shared_ptr<T> Load(const std::string& path)
	{
		return std::shared_ptr<T>();
	}
};

#define DEFINE_ASSET_LOADER(Type) template<> std::shared_ptr<Type> AssetLoader::Load(const std::string& path);

DEFINE_ASSET_LOADER(Mesh)
DEFINE_ASSET_LOADER(Shader)