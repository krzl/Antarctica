#pragma once

#include "Pathfinding/NavMesh.h"

class Mesh;

namespace Navigation
{
	class Terrain
	{
		friend class TerrainGenerator;

	public:

		typedef int16_t HeightLevel;

		HeightLevel GetHeight(uint32_t x, uint32_t y) const;

		void ConstructSubmesh(std::shared_ptr<Mesh> mesh, uint32_t xStart, uint32_t xEnd, uint32_t yStart, uint32_t yEnd) const;
		std::shared_ptr<Mesh> ConstructMesh() const;

		NavMesh CreateNavMesh() const;

		bool IsOnSlope(const Point3D& point) const;

	private:

		Terrain() = default;

		void SetHeight(uint32_t x, uint32_t y, HeightLevel level);

		uint32_t GetHeightMapArrayIndex(uint32_t x, uint32_t y) const;

		Point3D GetPos(uint32_t id) const;

		uint32_t GetHeightDifference(uint32_t a, uint32_t b, uint32_t c) const;

		NavMesh::Edge GenerateNavMeshEdge(uint8_t edgeDirection, std::vector<uint8_t>& lookup, uint32_t x, uint32_t y, int32_t deltaX,
										  int32_t deltaY) const;

		static float TerrainHeightLevelToZ(HeightLevel level);

		uint32_t m_width  = 0;
		uint32_t m_height = 0;
		std::vector<HeightLevel> m_heightMap;
		
		static constexpr float GRID_CELL_TO_METER         = 1.0f / 16.0f;
		static constexpr float GRID_LEVEL_HEIGHT_TO_METER = 5.0f;
	};
}
