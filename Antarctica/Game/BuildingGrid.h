#pragma once
#include "Assets/SubmeshData.h"

#include "Terrain/Terrain.h"

namespace Navigation
{
	class Terrain;
}

class BuildingGrid
{
	enum State
	{
		FREE,
		BLOCKED,
		USED
	};

public:

	explicit BuildingGrid(const Navigation::Terrain* terrain);

	Point2DInt GetGridPosition(const Point3D& point) const;

	bool IsFree(const Point2DInt& pos) const;

	bool IsFree(const Point2DInt& start, const Point2DInt& end) const;
	void SetUsed(const Point2DInt& start, const Point2DInt& end);
	void SetFree(const Point2DInt& start, const Point2DInt& end);

	void UpdatePlacementSubmesh(Submesh& submesh, const Point2DInt& start, const Point2DInt& end) const;
	
	uint32_t GetWidth() const { return m_terrain->GetWidth() - 1; }
	uint32_t GetHeight() const { return m_terrain->GetHeight() - 1; }

private:

	const Navigation::Terrain* m_terrain = nullptr;

	std::vector<State> m_grid;
};
