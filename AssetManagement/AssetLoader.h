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

template<>
std::shared_ptr<Mesh> AssetLoader::Load(const std::string& path);

template<>
std::shared_ptr<Shader> AssetLoader::Load(const std::string& path);