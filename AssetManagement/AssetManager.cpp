#include "stdafx.h"
#include "AssetManager.h"

std::unordered_map<uint64_t, std::weak_ptr<Asset>> AssetManager::m_assetDatabase = {};

void AssetManager::AddToAssetDatabase(const std::weak_ptr<Asset>& asset, const uint64_t id)
{
	m_assetDatabase[id] = asset;
}

uint64_t AssetManager::GetAssetId(const std::string& path)
{
	return static_cast<uint64_t>(std::hash<std::string>()(path));
}
