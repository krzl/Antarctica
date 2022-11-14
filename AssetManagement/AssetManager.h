#pragma once

#include "Asset.h"
#include "AssetLoader.h"

class AssetManager
{
public:

	template<
		typename T,
		typename = std::enable_if_t<std::is_base_of<Asset, T>::value>>
	static [[nodiscard]] std::shared_ptr<T> GetAsset(const std::string& path)
	{
		const uint64_t assetId = GetAssetId(path);

		const auto cachedAsset = m_assetDatabase.find(assetId);
		if (cachedAsset != m_assetDatabase.end() && !cachedAsset->second.expired())
		{
			const std::shared_ptr<T> asset = std::static_pointer_cast<T, Asset>(cachedAsset->second.lock());
			assert(asset.get() != nullptr);

			return asset;
		}

		std::shared_ptr<T> asset = AssetLoader::Load<T>(path);

		if (asset.get() != nullptr)
		{
			AddToAssetDatabase(std::static_pointer_cast<Asset, T>(asset), assetId);
		}
		else
		{
			Logger::Log("Could not load asset: %s", path);
		}

		return asset;
	}

private:

	static void AddToAssetDatabase(const std::weak_ptr<Asset>& asset, const uint64_t id);

	static uint64_t GetAssetId(const std::string& path);

	static std::unordered_map<uint64_t, std::weak_ptr<Asset>> m_assetDatabase;
};
