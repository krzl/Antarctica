#include "stdafx.h"
#include "TerrainGenerator.h"

std::shared_ptr<Terrain> TerrainGenerator::GenerateTerrain(const SpawnParameters& params)
{
	std::shared_ptr<Terrain> terrain(new Terrain);

	terrain->m_width  = params.m_width;
	terrain->m_height = params.m_height;

	terrain->m_heightMap.resize(terrain->m_width * terrain->m_height, 0);

	const uint32_t islandCount = Random::GetRandomUInt32(params.m_minIslandCount, params.m_maxIslandCount);
	for (uint32_t i = 0; i < islandCount; ++i)
	{
		constexpr uint32_t retryCount = 10;
		for (uint32_t j = 0; j < retryCount; ++j)
		{
			const uint32_t x = Random::GetRandomUInt32(0, terrain->m_width - 1);
			const uint32_t y = Random::GetRandomUInt32(0, terrain->m_height - 1);

			if (terrain->GetHeight(x, y) == 0)
			{
				SpawnRandomIsland(*terrain, params, x, y, 5);
			}
		}
	}

	return terrain;
}

void TerrainGenerator::SpawnRandomIsland(Terrain& terrain, const SpawnParameters& params, const uint32_t startPosX,
										 const uint32_t startPosY, const Terrain::HeightLevel level)
{
	for (uint32_t i = 0; i < 2; ++i)
	{
		const int32_t startingHeight = Random::GetRandomUInt32(params.m_minIslandStartingSize,
															   params.m_maxIslandStartingSize);
		for (uint32_t j = 0; j < 2; ++j)
		{
			int32_t        currentHeight = startingHeight;
			const uint32_t width         = Random::GetRandomInt32(params.m_minIslandStartingSize,
																  params.m_maxIslandStartingSize);

			for (uint32_t k = 0; k < width; ++k)
			{
				if (k != 0)
				{
					const float heightWidthRatio = (float) currentHeight / (width - k);
					const float slopeRatio       = LerpClamped(1.0f, params.m_slopeRatio, (float) k / (width - 1));
					currentHeight += Random::GetRandomInt32(
						heightWidthRatio * slopeRatio * -params.m_heightDegradationRate,
						params.m_maxHeightIncrease);
				}
				if (currentHeight <= 0)
				{
					break;
				}
				for (uint32_t l = 0; l < (uint32_t) currentHeight; ++l)
				{
					const uint32_t posX = startPosX + (i == 0 ? 1 : -1) * k;
					const uint32_t posY = startPosY + (j == 0 ? 1 : -1) * l;
					terrain.SetHeight(posX, posY, level);
				}
			}
		}
	}
}
