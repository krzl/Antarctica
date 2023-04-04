#pragma once

#include "Assets/Animation.h"
#include "Assets/ComputeShader.h"
#include "Assets/Mesh.h"
#include "Assets/Shader.h"
#include "Assets/Texture.h"

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

DEFINE_ASSET_LOADER(Animation)
DEFINE_ASSET_LOADER(ComputeShader)
DEFINE_ASSET_LOADER(Mesh)
DEFINE_ASSET_LOADER(Shader)
DEFINE_ASSET_LOADER(Texture)
