#include "stdafx.h"
#include "AssetManager.h"

std::unordered_map<uint64_t, std::weak_ptr<Asset>> AssetManager::m_assetDatabase = {};

uint64_t AssetManager::GetAssetId(const std::string& path)
{
	return static_cast<uint64_t>(std::hash<std::string>()(path));
}
