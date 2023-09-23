#include "stdafx.h"
#include "TerrainLoader.h"

#include "AssetManager.h"

#include "Assets/Texture.h"

namespace Navigation
{
	std::unique_ptr<Terrain> TerrainLoader::Load(const std::string& path)
	{
		const std::shared_ptr<Texture> texture = AssetManager::GetAsset<Texture>(path);

		if (texture == nullptr)
		{
			return nullptr;
		}

		if (texture->GetChannels() < 3)
		{
			return nullptr;
		}

		std::unique_ptr<Terrain> terrain = std::unique_ptr<Terrain>(new Terrain);

		terrain->m_width  = texture->GetWidth();
		terrain->m_height = texture->GetHeight();

		terrain->m_heightMap.resize(terrain->m_width * terrain->m_height, 0);

		uint32_t id = 0;

		for (uint32_t y = 0; y < terrain->m_width; ++y)
		{
			for (uint32_t x = 0; x < terrain->m_height; ++x)
			{
				const uint8_t r = texture->GetData()[id * 4 + 0];
				const uint8_t g = texture->GetData()[id * 4 + 1];
				const uint8_t b = texture->GetData()[id * 4 + 2];

				++id;

				const Terrain::HeightLevel heightLevel =
					(Terrain::HeightLevel) round(
						r / 255.0f * -5 +
						g / 255.0f * 0 +
						b / 255.0f * 5);

				terrain->m_heightMap[terrain->GetHeightMapArrayIndex(x, terrain->m_height - y - 1)] = heightLevel;
			}
		}

		return terrain;
	}
}
