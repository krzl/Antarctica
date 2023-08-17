#pragma once

#include "Terrain.h"

namespace Navigation
{
	class TerrainGenerator
	{
	public:

		struct SpawnParameters
		{
			uint32_t m_width  = 512;
			uint32_t m_height = 512;

			uint32_t m_minIslandCount = 14;
			uint32_t m_maxIslandCount = 23;

			uint32_t m_minIslandStartingSize = 10;
			uint32_t m_maxIslandStartingSize = 20;

			uint32_t m_maxHeightIncrease = 1;

			float m_heightDegradationRate = 2.0f;
			float m_slopeRatio            = 3.0f;
		};

		static void SpawnRandomIsland(Terrain& terrain, const SpawnParameters& params, uint32_t startPosX, uint32_t startPosY,
									  Terrain::HeightLevel level);

		static std::shared_ptr<Terrain> GenerateTerrain(const SpawnParameters& params);
	};
}
