#include "stdafx.h"
#include "Terrain.h"

#include "Debug/DebugDrawManager.h"

namespace Navigation
{
	enum NavMeshEdgeDirection
	{
		UP         = 1 << 0,
		LEFT       = 1 << 1,
		RIGHT      = 1 << 2,
		DOWN       = 1 << 3,
		UP_LEFT    = 1 << 4,
		UP_RIGHT   = 1 << 5,
		DOWN_LEFT  = 1 << 6,
		DOWN_RIGHT = 1 << 7
	};

	NavMesh::Edge Terrain::GenerateNavMeshEdge(const uint8_t edgeDirection, std::vector<uint8_t>& lookup, const uint32_t x,
											   const uint32_t y,
											   const int32_t deltaX, const int32_t deltaY) const
	{
		int32_t nextX = x + deltaX;
		int32_t nextY = y + deltaY;

		while (nextX >= 0 && nextY >= 0 && nextX < (int32_t) m_width && nextY < (int32_t) m_height &&
			lookup[GetHeightMapArrayIndex(nextX, nextY)] == edgeDirection)
		{
			lookup[GetHeightMapArrayIndex(nextX, nextY)] = 0;

			nextX += deltaX;
			nextY += deltaY;
		}

		if (nextX < 0 || nextY < 0 || nextX >= (int32_t) m_width || nextY >= (int32_t) m_height)
		{
			nextX -= deltaX;
			nextY -= deltaY;
		}

		return NavMesh::Edge{
			GetHeightMapArrayIndex(x, y),
			GetHeightMapArrayIndex(nextX, nextY)
		};
	}

	NavMesh Terrain::CreateNavMesh() const
	{
		const uint32_t vertexCount = m_width * m_height;

		std::vector<uint8_t> lookup(vertexCount);

		for (uint32_t x = 0; x < m_width; ++x)
		{
			for (uint32_t y = 0; y < m_height; ++y)
			{
				const uint32_t a1 = GetHeightMapArrayIndex(x, y);

				if (x != m_width - 1 && y != m_height - 1)
				{
					const uint32_t a2 = GetHeightMapArrayIndex(x, y + 1);
					const uint32_t b1 = GetHeightMapArrayIndex(x + 1, y);
					const uint32_t b2 = GetHeightMapArrayIndex(x + 1, y + 1);

					const bool isLeftTraversable  = GetHeightDifference(a1, a2, b1) < 2;
					const bool isRightTraversable = GetHeightDifference(b1, a2, b2) < 2;

					if (!isLeftTraversable)
					{
						if (m_heightMap[a1] == m_heightMap[a2])
						{
							lookup[a1] |= UP;
							lookup[a2] |= DOWN;
						}
						if (m_heightMap[a2] == m_heightMap[b1])
						{
							lookup[a2] |= DOWN_RIGHT;
							lookup[b1] |= UP_LEFT;
						}
						if (m_heightMap[b1] == m_heightMap[a1])
						{
							lookup[b1] |= LEFT;
							lookup[a1] |= RIGHT;
						}
					}
					if (!isRightTraversable)
					{
						if (m_heightMap[b1] == m_heightMap[a2])
						{
							lookup[a2] |= DOWN_RIGHT;
							lookup[b1] |= UP_LEFT;
						}
						if (m_heightMap[a2] == m_heightMap[b2])
						{
							lookup[a2] |= RIGHT;
							lookup[b2] |= LEFT;
						}
						if (m_heightMap[b2] == m_heightMap[b1])
						{
							lookup[b2] |= DOWN;
							lookup[b1] |= UP;
						}
					}
				}
			}
		}

		for (uint32_t i = 0; i < m_width - 1; ++i)
		{
			const uint32_t a1 = GetHeightMapArrayIndex(i, 0);
			const uint32_t a2 = GetHeightMapArrayIndex(i + 1, 0);

			const uint32_t b1 = GetHeightMapArrayIndex(i, m_height - 1);
			const uint32_t b2 = GetHeightMapArrayIndex(i + 1, m_height - 1);

			if (m_heightMap[a1] == m_heightMap[a2])
			{
				lookup[a1] |= RIGHT;
				lookup[a2] |= LEFT;
			}

			if (m_heightMap[b1] == m_heightMap[b2])
			{
				lookup[b1] |= RIGHT;
				lookup[b2] |= LEFT;
			}
		}

		for (uint32_t i = 0; i < m_height - 1; ++i)
		{
			const uint32_t a1 = GetHeightMapArrayIndex(0, i);
			const uint32_t a2 = GetHeightMapArrayIndex(0, i + 1);

			const uint32_t b1 = GetHeightMapArrayIndex(m_width - 1, i);
			const uint32_t b2 = GetHeightMapArrayIndex(m_width - 1, i + 1);

			if (m_heightMap[a1] == m_heightMap[a2])
			{
				lookup[a1] |= UP;
				lookup[a2] |= DOWN;
			}

			if (m_heightMap[b1] == m_heightMap[b2])
			{
				lookup[b1] |= UP;
				lookup[b2] |= DOWN;
			}
		}

		std::vector<NavMesh::Edge> edges;

		for (uint32_t x = 0; x < m_width; ++x)
		{
			for (uint32_t y = 0; y < m_height; ++y)
			{
				const uint32_t a1 = GetHeightMapArrayIndex(x, y);

				const uint8_t usedEdges = lookup[a1];

				if (usedEdges == 0)
				{
					continue;
				}

				if (usedEdges & RIGHT)
				{
					edges.emplace_back(GenerateNavMeshEdge(RIGHT | LEFT, lookup, x, y, 1, 0));
				}

				if (usedEdges & UP)
				{
					edges.emplace_back(GenerateNavMeshEdge(UP | DOWN, lookup, x, y, 0, 1));
				}

				if (usedEdges & UP_LEFT)
				{
					edges.emplace_back(GenerateNavMeshEdge(UP_LEFT | DOWN_RIGHT, lookup, x, y, -1, 1));
				}

				if (usedEdges & UP_RIGHT)
				{
					edges.emplace_back(GenerateNavMeshEdge(UP_RIGHT | DOWN_LEFT, lookup, x, y, 1, 1));
				}
			}
		}

		std::map<uint32_t, uint32_t> vertexUsageCount;

		for (NavMesh::Edge& edge : edges)
		{
			++vertexUsageCount[edge.m_start];
			++vertexUsageCount[edge.m_end];
		}

		bool hasEdgesChanged = true;

		while (hasEdgesChanged)
		{
			hasEdgesChanged = false;

			for (auto it = edges.begin(); it != edges.end();)
			{
				NavMesh::Edge& edge = *it;
				if (vertexUsageCount[edge.m_start] == 1 || vertexUsageCount[edge.m_end] == 1)
				{
					--vertexUsageCount[edge.m_start];
					--vertexUsageCount[edge.m_end];

					hasEdgesChanged = true;

					it = edges.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		std::vector<Point3D> vertices;
		std::map<uint32_t, uint32_t> verticesIdMap;

		for (auto [id, usageCount] : vertexUsageCount)
		{
			if (usageCount != 0)
			{
				vertices.emplace_back(GetPos(id));
				verticesIdMap[id] = (uint32_t) vertices.size() - 1;
			}
		}

		std::vector<NavMesh::Edge> finalEdges;
		finalEdges.reserve(edges.size());

		for (const NavMesh::Edge& edge : edges)
		{
			finalEdges.emplace_back(NavMesh::Edge{ verticesIdMap[edge.m_start], verticesIdMap[edge.m_end] });
		}

		return NavMesh(std::move(vertices), std::move(finalEdges), *this);
	}
}
