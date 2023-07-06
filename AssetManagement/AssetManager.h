#pragma once

#include "Assets/Asset.h"

class AssetManager
{
public:

	template<
		typename T,
		typename = std::enable_if_t<std::is_base_of_v<Asset, T>>>
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

		std::shared_ptr<T> asset = std::make_shared<T>();

		const std::shared_ptr<Asset> assetBase = std::static_pointer_cast<Asset, T>(asset);

		if (assetBase->Load(path))
		{
			AddToAssetDatabase(assetBase, assetId);
		}
		else
		{
			LOG(DEBUG, "AssetManager", "Could not load asset: {}", path);
		}

		return asset;
	}

private:

	static void AddToAssetDatabase(const std::weak_ptr<Asset>& asset, const uint64_t id);

	static uint64_t GetAssetId(const std::string& path);

	static std::unordered_map<uint64_t, std::weak_ptr<Asset>> m_assetDatabase;
};
