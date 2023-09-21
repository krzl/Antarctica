#include "stdafx.h"
#include "BuildingGrid.h"

#include "Terrain/Terrain.h"

BuildingGrid::BuildingGrid(const Navigation::Terrain* terrain)
{
	m_terrain = terrain;

	m_grid.reserve((terrain->GetHeight() - 1) * (terrain->GetWidth() - 1));
	for (uint32_t y = 0; y < terrain->GetHeight() - 1; ++y)
	{
		for (uint32_t x = 0; x < terrain->GetWidth() - 1; ++x)
		{
			const Navigation::Terrain::HeightLevel a = terrain->GetHeightLevel(x, y);
			const Navigation::Terrain::HeightLevel b = terrain->GetHeightLevel(x + 1, y);
			const Navigation::Terrain::HeightLevel c = terrain->GetHeightLevel(x, y + 1);
			const Navigation::Terrain::HeightLevel d = terrain->GetHeightLevel(x + 1, y + 1);

			m_grid.emplace_back(a == b && b == c && c == d ? FREE : BLOCKED);
		}
	}
}

Point2DInt BuildingGrid::GetGridPosition(const Point3D& point) const
{
	const Point3D leftBotCorner = m_terrain->GetPos(0, 0);

	const Point3D remappedPos = (Point3D) (point - leftBotCorner) / Navigation::Terrain::GRID_CELL_TO_METER;

	return Point2DInt{
		Clamp<int32_t>(0, m_terrain->GetWidth() - 1, (int32_t) Floor(remappedPos.x)),
		Clamp<int32_t>(0, m_terrain->GetHeight() - 1, (int32_t) Floor(remappedPos.y)),
	};
}

bool BuildingGrid::IsFree(const Point2DInt& pos) const
{
	return m_grid[pos.x + pos.y * (m_terrain->GetWidth() - 1)] == FREE;
}

bool BuildingGrid::IsFree(const Point2DInt& start, const Point2DInt& end) const
{
	const Point2DInt center = start + (end - start) / 2;

	const Navigation::Terrain::HeightLevel level = m_terrain->GetHeightLevel(center.x, center.y);

	for (int32_t x = start.x; x <= end.x; ++x)
	{
		for (int32_t y = start.y; y <= end.y; ++y)
		{
			if (level != m_terrain->GetHeightLevel(x, y))
			{
				return false;
			}

			if (IsFree(Point2DInt{ x, y }))
			{
				return false;
			}
		}
	}

	return true;
}

void BuildingGrid::SetFree(const Point2DInt& start, const Point2DInt& end)
{
	for (int32_t x = start.x; x <= end.x; ++x)
	{
		for (int32_t y = start.y; y <= end.y; ++y)
		{
			m_grid[x + y * (m_terrain->GetWidth() - 1)] = FREE;
		}
	}
}

void BuildingGrid::UpdatePlacementSubmesh(Submesh& submesh, const Point2DInt& start, const Point2DInt& end) const
{
	const uint32_t segmentCount = (end.x - start.x + 1) * (end.y - start.y + 1);

	const Point2DInt center = start + (end - start) / 2;

	const Navigation::Terrain::HeightLevel level = m_terrain->GetHeightLevel(center.x, center.y);

	MeshBuffer& vertexBuffer   = submesh.GetVertexBuffer();
	vertexBuffer.m_elementSize = sizeof(Point3D) + sizeof(Color);
	vertexBuffer.m_data.resize(vertexBuffer.m_elementSize * segmentCount * 4);

	MeshBuffer& indexBuffer   = submesh.GetIndexBuffer();
	indexBuffer.m_elementSize = sizeof(uint32_t);
	indexBuffer.m_data.resize(indexBuffer.m_elementSize * segmentCount * 6);

	void* vertexData    = vertexBuffer.m_data.data();
	uint32_t* indexData = reinterpret_cast<uint32_t*>(indexBuffer.m_data.data());

	uint32_t currentSegmentId = 0;

	Point3D* pointArray = static_cast<Point3D*>(vertexData);
	Color* colorArray   = reinterpret_cast<Color*>(static_cast<uint8_t*>(vertexData) + sizeof(Point3D) * vertexBuffer.GetElementCount());


	for (int32_t x = start.x; x <= end.x; ++x)
	{
		for (int32_t y = start.y; y <= end.y; ++y)
		{
			const bool isPlaceable = level == m_terrain->GetHeightLevel(x, y) && IsFree(Point2DInt{ x, y });

			Point3D& a = pointArray[currentSegmentId * 4 + 0];
			Point3D& b = pointArray[currentSegmentId * 4 + 1];
			Point3D& c = pointArray[currentSegmentId * 4 + 2];
			Point3D& d = pointArray[currentSegmentId * 4 + 3];

			a   = m_terrain->GetPos(x, y);
			b   = m_terrain->GetPos(x + 1, y);
			c   = m_terrain->GetPos(x, y + 1);
			d   = m_terrain->GetPos(x + 1, y + 1);
			
			a.z = b.z = c.z = d.z = m_terrain->HeightLevelToZ(level);

			Color& aColor = colorArray[currentSegmentId * 4 + 0];
			Color& bColor = colorArray[currentSegmentId * 4 + 1];
			Color& cColor = colorArray[currentSegmentId * 4 + 2];
			Color& dColor = colorArray[currentSegmentId * 4 + 3];

			aColor = bColor = cColor = dColor = isPlaceable ? Color::green : Color::red;

			indexData[currentSegmentId * 6 + 0] = currentSegmentId * 4 + 0;
			indexData[currentSegmentId * 6 + 1] = currentSegmentId * 4 + 2;
			indexData[currentSegmentId * 6 + 2] = currentSegmentId * 4 + 3;
			indexData[currentSegmentId * 6 + 3] = currentSegmentId * 4 + 3;
			indexData[currentSegmentId * 6 + 4] = currentSegmentId * 4 + 1;
			indexData[currentSegmentId * 6 + 5] = currentSegmentId * 4 + 0;

			++currentSegmentId;
		}
	}
}

void BuildingGrid::SetUsed(const Point2DInt& start, const Point2DInt& end)
{
	for (int32_t x = start.x; x <= end.x; ++x)
	{
		for (int32_t y = start.y; y <= end.y; ++y)
		{
			m_grid[x + y * (m_terrain->GetWidth() - 1)] = USED;
		}
	}
}
