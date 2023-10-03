#pragma once

#include "Assets/BVH.h"

#include "Pathfinding/NavMesh.h"

class Mesh;

namespace Navigation
{
	class Terrain
	{
		friend class TerrainGenerator;
		friend class TerrainLoader;

	public:

		typedef int16_t HeightLevel;

		HeightLevel GetHeightLevel(uint32_t x, uint32_t y) const;

		[[nodiscard]] uint32_t GetWidth() const { return m_width; }
		[[nodiscard]] uint32_t GetHeight() const { return m_height; }

		Point3D GetPos(uint32_t x, uint32_t y) const;
		Point3D GetPos(uint32_t id) const;

		float GetHeightAtLocation(const Point2D& point) const;

		void ConstructSubmesh(std::shared_ptr<Mesh> mesh, uint32_t xStart, uint32_t xEnd, uint32_t yStart, uint32_t yEnd) const;
		std::shared_ptr<Mesh> ConstructMesh();

		NavMesh* CreateNavMesh() const;

		bool IsOnSlope(const Point3D& point) const;

		std::optional<Point3D> Intersect(const Ray& ray) const;

		static float HeightLevelToZ(HeightLevel level);

		static constexpr float GRID_CELL_TO_METER         = 1.0f / 1.0f;
		static constexpr float GRID_LEVEL_HEIGHT_TO_METER = 5.0f;

	private:

		Terrain() = default;

		void SetHeight(uint32_t x, uint32_t y, HeightLevel level);

		uint32_t GetHeightMapArrayIndex(uint32_t x, uint32_t y) const;

		uint32_t GetHeightDifference(uint32_t a, uint32_t b, uint32_t c) const;

		NavMesh::Edge GenerateNavMeshEdge(uint8_t edgeDirection, std::vector<uint8_t>& lookup, uint32_t x, uint32_t y, int32_t deltaX,
										  int32_t deltaY) const;

		uint32_t m_width  = 0;
		uint32_t m_height = 0;
		std::vector<HeightLevel> m_heightMap;

		BVH m_bvh;
	};
}
