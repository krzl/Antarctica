#include "stdafx.h"
#include "NavMesh.h"

#include "Assets/SubmeshData.h"

#include "Debug/DebugDrawManager.h"

#include "Terrain/Terrain.h"

namespace Navigation
{
	NavMesh::NavMesh(const std::vector<Point3D>& vertices, std::vector<Edge>&& edges, const Terrain& terrain)
	{
		m_edges = std::move(edges);

		const uint32_t vertexCount = (uint32_t) vertices.size();
		m_vertices.reserve(vertexCount + 3);

		m_vertexToTriangleMap.reserve(vertexCount + 3);
		m_vertexToTriangleMap.resize(3);

		m_vertices.emplace_back(Point3D(-10000.0f, -10000.0f, 0.0f));
		m_vertices.emplace_back(Point3D(0.0f, 10000.0f, 0.0f));
		m_vertices.emplace_back(Point3D(10000.0f, -10000.0f, 0.0f));

		m_triangles.reserve(vertices.size() * 2 + 1);
		m_triangles.emplace_back();

		SetTriangle(0, Triangle{
			{
				0,
				1,
				2
			},
			{
				Triangle::NULL_ID,
				Triangle::NULL_ID,
				Triangle::NULL_ID
			}
		});

		for (uint32_t i = 0; i < vertexCount; ++i)
		{
			AddVertex(vertices[i]);
		}

		for (uint32_t i = 0; i < m_edges.size(); ++i)
		{
			AddConstraint(Edge{ m_edges[i].m_start + 3, m_edges[i].m_end + 3 });
		}

		for (uint32_t i = 0; i < m_triangles.size(); ++i)
		{
			Triangle& triangle = m_triangles[i];

			Point3D center = (m_vertices[triangle.m_vertices[0]] +
				m_vertices[triangle.m_vertices[1]] +
				m_vertices[triangle.m_vertices[2]]) / 3.0f;

			triangle.m_isNavigable = !terrain.IsOnSlope(center);
		}

		/*
		{
			Color color[] = {
				Color::black,
				Color::blue,
				Color::cyan,
				Color::green,
				Color::magenta,
				Color::red,
				Color::yellow,
				Color::white
			};
		
			for (uint32_t i = 0; i < m_triangles.size(); ++i)
			{
				std::vector<Point3D> vertexList;
				vertexList.reserve(3);
		
				const Triangle& triangle = m_triangles[i];
				if (triangle.m_isNavigable)
				{
					const Vector3D a = m_vertices[triangle.m_vertices[0]];
					const Vector3D b = m_vertices[triangle.m_vertices[1]];
					const Vector3D c = m_vertices[triangle.m_vertices[2]];
		
					vertexList.emplace_back(a + Vector3D(0.0f, 0.0f, 0.05f));
					vertexList.emplace_back(b + Vector3D(0.0f, 0.0f, 0.05f));
					vertexList.emplace_back(c + Vector3D(0.0f, 0.0f, 0.05f));
		
					DebugDrawManager::GetInstance()->DrawTriangles(vertexList, 1000.0f, color[i % 8]);
				}
			}
		}*/
	}

	void NavMesh::AddVertex(const Point3D& vertex)
	{
		const uint32_t vertexId   = (uint32_t) m_vertices.size();
		const uint32_t triangleId = FindTriangleId(vertex);
		m_vertices.emplace_back(vertex);
		m_vertexToTriangleMap.emplace_back();

		const Triangle oldTriangle = m_triangles[triangleId];

		//TODO: try to get index of triangle that is already allocated but no longer exists
		const uint32_t newTriangleIds[3] = { triangleId, (uint32_t) m_triangles.size(), (uint32_t) m_triangles.size() + 1 };

		m_triangles.resize(newTriangleIds[2] + 1);

		std::stack<uint32_t> triangleIdsToVisit;
		std::set<uint32_t> awaitingTrianglesToVisit;

		RemoveTriangle(triangleId);

		for (uint32_t i = 0; i < 3; ++i)
		{
			SetTriangle(newTriangleIds[i], Triangle{
				{
					oldTriangle.m_vertices[i],
					oldTriangle.m_vertices[(i + 1) % 3],
					vertexId
				},
				{
					oldTriangle.m_adjacentTriangles[i],
					newTriangleIds[(i + 1) % 3],
					newTriangleIds[(i + 2) % 3]
				}
			});

			for (uint32_t j = 0; j < 3; ++j)
			{
				if (oldTriangle.m_adjacentTriangles[i] != Triangle::NULL_ID &&
					m_triangles[oldTriangle.m_adjacentTriangles[i]].m_adjacentTriangles[j] == triangleId)
				{
					m_triangles[oldTriangle.m_adjacentTriangles[i]].m_adjacentTriangles[j] = newTriangleIds[i];
					break;
				}
			}

			if (oldTriangle.m_adjacentTriangles[i] != Triangle::NULL_ID)
			{
				triangleIdsToVisit.emplace(oldTriangle.m_adjacentTriangles[i]);
				awaitingTrianglesToVisit.emplace(oldTriangle.m_adjacentTriangles[i]);
			}
		}

		while (!triangleIdsToVisit.empty())
		{
			const uint32_t currentTriangleId = triangleIdsToVisit.top();
			const Triangle& currentTriangle  = m_triangles[currentTriangleId];
			triangleIdsToVisit.pop();
			awaitingTrianglesToVisit.erase(currentTriangleId);

			uint32_t currentAdjacentTriangleId = Triangle::NULL_ID;
			for (uint32_t x = 0; x < 9; ++x)
			{
				const uint32_t i = x / 3;
				const uint32_t j = x % 3;

				if (currentTriangle.m_adjacentTriangles[i] == newTriangleIds[j])
				{
					currentAdjacentTriangleId = newTriangleIds[j];
					break;
				}
			}

			if (currentAdjacentTriangleId == Triangle::NULL_ID)
			{
				continue;
			}

			const uint32_t oppositeVertexId = FindOppositeSideVertexId(currentTriangle, m_triangles[currentAdjacentTriangleId]);

			if (TriangleFlipTest(vertexId, currentTriangle, oppositeVertexId))
			{
				for (uint32_t i = 0; i < 3; ++i)
				{
					if (currentTriangle.m_adjacentTriangles[i] != currentAdjacentTriangleId &&
						currentTriangle.m_adjacentTriangles[i] != Triangle::NULL_ID &&
						awaitingTrianglesToVisit.count(currentTriangle.m_adjacentTriangles[i]) == 0)
					{
						triangleIdsToVisit.emplace(currentTriangle.m_adjacentTriangles[i]);
						awaitingTrianglesToVisit.emplace(currentTriangle.m_adjacentTriangles[i]);
					}
				}

				FlipDiagonal(currentTriangleId, currentAdjacentTriangleId, vertexId, oppositeVertexId);
			}
		}
	}

	static uint32_t GetSegmentOrientation(const Point2D& a, const Point2D& b, const Point2D& c)
	{
		const float value = (b.y - a.y) * (c.x - b.x) - (b.x - a.x) * (c.y - b.y);

		if (value == 0.0f)
		{
			return 0;
		}

		return value > 0 ? 1 : 2;
	}

	static uint32_t GetSegmentOrientation(const Point3D& a, const Point3D& b, const Point3D& c)
	{
		return GetSegmentOrientation(Point2D(a.x, a.y), Point2D(b.x, b.y), Point2D(c.x, c.y));
	}

	void NavMesh::SetTriangle(const uint32_t triangleId, const Triangle& triangle)
	{
		m_triangles[triangleId] = triangle;

		for (uint32_t i = 0; i < 3; ++i)
		{
			m_vertexToTriangleMap[triangle.m_vertices[i]].insert(triangleId);
		}
	}

	void NavMesh::RemoveTriangle(const uint32_t triangleId)
	{
		const Triangle& triangle = m_triangles[triangleId];

		for (uint32_t i = 0; i < 3; ++i)
		{
			m_vertexToTriangleMap[triangle.m_vertices[i]].erase(triangleId);
		}
	}

	uint32_t NavMesh::FindOppositeSideVertexId(const Triangle& searchTriangle, const Triangle& compareTriangle)
	{
		for (uint32_t i = 0; i < 3; ++i)
		{
			bool wasFound = false;
			for (uint32_t j = 0; j < 3; ++j)
			{
				if (searchTriangle.m_vertices[i] == compareTriangle.m_vertices[j])
				{
					wasFound = true;
				}
			}

			if (!wasFound)
			{
				return searchTriangle.m_vertices[i];
			}
		}

		assert(false);

		return 0xFFFFFFFF;
	}

	bool IsOutsideOfEdge(const Point3D& a, const Point3D& b, const Point3D& p)
	{
		return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x) > 0.0f;
	}

	uint32_t NavMesh::FindTriangleId(const Point3D& vertex) const
	{
		uint32_t currentTriangleId = (uint32_t) m_triangles.size() - 1;

		while (true)
		{
			const Triangle& triangle = m_triangles[currentTriangleId];

			const Point3D& a = m_vertices[triangle.m_vertices[0]];
			const Point3D& b = m_vertices[triangle.m_vertices[1]];
			const Point3D& c = m_vertices[triangle.m_vertices[2]];

			const bool outsideEdgeA = IsOutsideOfEdge(a, b, vertex);
			const bool outsideEdgeB = IsOutsideOfEdge(b, c, vertex);
			const bool outsideEdgeC = IsOutsideOfEdge(c, a, vertex);

			if (!outsideEdgeA && !outsideEdgeB && !outsideEdgeC)
			{
				return currentTriangleId;
			}

			if (outsideEdgeA)
			{
				currentTriangleId = triangle.m_adjacentTriangles[0];
			}
			else if (outsideEdgeB)
			{
				currentTriangleId = triangle.m_adjacentTriangles[1];
			}
			else if (outsideEdgeC)
			{
				currentTriangleId = triangle.m_adjacentTriangles[2];
			}
		}
	}

	uint32_t NavMesh::GetAdjacentTriangleId(const std::vector<unsigned>& adjacentTriangles, const std::vector<unsigned>::iterator& it,
											const bool seekLeft)
	{
		if (seekLeft)
		{
			if (it == adjacentTriangles.begin())
			{
				return adjacentTriangles[adjacentTriangles.size() - 1];
			}
			return *(it - 1);
		}
		else
		{
			if (it + 1 == adjacentTriangles.end())
			{
				return *adjacentTriangles.begin();
			}
			return *(it + 1);
		}
	}

	void NavMesh::GetNextVerticesClockwise(const Triangle& triangle, const uint32_t startingVertexId, uint32_t& p1, uint32_t& p2)
	{
		for (uint32_t i = 0; i < 3; ++i)
		{
			if (triangle.m_vertices[i] == startingVertexId)
			{
				p1 = triangle.m_vertices[(i + 1) % 3];
				p2 = triangle.m_vertices[(i + 2) % 3];
				return;
			}
		}
	}

	bool NavMesh::TriangleFlipTest(const uint32_t vertexId, const Triangle& triangle, const uint32_t oppositeVertexId) const
	{
		/*
		const Vector3D a = m_vertices[triangle.m_vertices[0]];
		const Vector3D b = m_vertices[triangle.m_vertices[1]];
		const Vector3D c = m_vertices[triangle.m_vertices[2]];

		float xx = sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
		float yy = sqrt((c.x - a.x) * (c.x - a.x) + (c.y - a.y) * (c.y - a.y));
		float cc = sqrt((c.x - b.x) * (c.x - b.x) + (c.y - b.y) * (c.y - b.y));

		float radius = (xx * yy * cc) / (sqrt((xx + yy + cc) * (yy + cc - xx) * (cc + xx - yy) * (xx + yy - cc)));
		float d      = 2.0f * (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y));
		float xp     = ((a.x * a.x + a.y * a.y) * (b.y - c.y) + (b.x * b.x + b.y * b.y) * (c.y - a.y) + (c.x * c.x + c.y * c.y) * (a.y - b.y)) /
			d;
		float yp = ((a.x * a.x + a.y * a.y) * (c.x - b.x) + (b.x * b.x + b.y * b.y) * (a.x - c.x) + (c.x * c.x + c.y * c.y) * (b.x - a.x)) /
			d;

		const Point2D circumcircle(xp, yp);

		const float diffA = Magnitude(a.xy - circumcircle);
		const float diffB = Magnitude(b.xy - circumcircle);
		const float diffC = Magnitude(c.xy - circumcircle);

		const float mag = Magnitude(circumcircle - m_vertices[vertexId].xy);

		return mag < diffA;
		*/


		uint32_t p1Id;
		uint32_t p2Id;

		GetNextVerticesClockwise(triangle, oppositeVertexId, p2Id, p1Id);

		const Point3D& p1 = m_vertices[p1Id];
		const Point3D& p2 = m_vertices[p2Id];
		const Point3D& p3 = m_vertices[oppositeVertexId];
		const Point3D& p  = m_vertices[vertexId];

		const float x13 = p1.x - p3.x;
		const float x23 = p2.x - p3.x;
		const float x1P = p1.x - p.x;
		const float x2P = p2.x - p.x;

		const float y13 = p1.y - p3.y;
		const float y23 = p2.y - p3.y;
		const float y1P = p1.y - p.y;
		const float y2P = p2.y - p.y;

		const float cosA = x13 * x23 + y13 * y23;
		const float cosB = x2P * x1P + y2P * y1P;

		if (cosA >= 0.0f && cosB >= 0.0f)
		{
			return false;
		}

		if (cosA < 0.0f && cosB < 0.0f)
		{
			return true;
		}

		const float sinAB = (x13 * y23 - x23 * y13) * cosB + (x2P * y1P - x1P * y2P) * cosA;

		return sinAB < 0;
	}

	void NavMesh::GetEdgeTriangles(const Edge& edge, uint32_t& aTriangleId, uint32_t& bTriangleId) const
	{
		aTriangleId = 0xFFFFFFFF;
		bTriangleId = 0xFFFFFFFF;

		const std::unordered_set<unsigned>& triangleMap1 = m_vertexToTriangleMap[edge.m_start];
		for (uint32_t triangleId : triangleMap1)
		{
			const std::unordered_set<unsigned>& triangleMap2 = m_vertexToTriangleMap[edge.m_end];
			if (triangleMap2.find(triangleId) != triangleMap2.end())
			{
				if (aTriangleId == 0xFFFFFFFF)
				{
					aTriangleId = triangleId;
				}
				else
				{
					bTriangleId = triangleId;
					break;
				}
			}
		}

		const Triangle& aTriangle = m_triangles[aTriangleId];
		for (uint32_t i = 0; i < 3; ++i)
		{
			if (aTriangle.m_vertices[i] == edge.m_start)
			{
				if (aTriangle.m_vertices[(i + 1) % 3] != edge.m_end)
				{
					std::swap(aTriangleId, bTriangleId);
				}

				return;
			}
		}
	}

	void NavMesh::FlipDiagonal(const uint32_t aTriangleId, const uint32_t bTriangleId, uint32_t aVertexId, uint32_t bVertexId)
	{
		const Triangle& currentTriangle        = m_triangles[aTriangleId];
		const Triangle& vertexAdjacentTriangle = m_triangles[bTriangleId];

		if (aVertexId == Triangle::NULL_ID)
		{
			aVertexId = FindOppositeSideVertexId(vertexAdjacentTriangle, currentTriangle);
		}

		if (bVertexId == Triangle::NULL_ID)
		{
			bVertexId = FindOppositeSideVertexId(currentTriangle, vertexAdjacentTriangle);
		}

		uint32_t localOppositeVertexId = Triangle::NULL_ID;
		for (uint32_t i = 0; i < 3; ++i)
		{
			if (vertexAdjacentTriangle.m_vertices[i] == aVertexId)
			{
				localOppositeVertexId = i;
				break;
			}
		}

		uint32_t localCurrentVertexId = Triangle::NULL_ID;
		for (uint32_t i = 0; i < 3; ++i)
		{
			if (currentTriangle.m_vertices[i] == bVertexId)
			{
				localCurrentVertexId = i;
				break;
			}
		}

		const uint32_t i1 = (localCurrentVertexId + 1) % 3;
		const uint32_t i2 = (localCurrentVertexId + 2) % 3;

		const uint32_t j1 = (localOppositeVertexId + 1) % 3;
		const uint32_t j2 = (localOppositeVertexId + 2) % 3;

		const uint32_t aSideVertexId            = currentTriangle.m_vertices[i1];
		const uint32_t aAdjacentTriangleId      = currentTriangle.m_adjacentTriangles[localCurrentVertexId];
		const uint32_t aOtherAdjacentTriangleId = vertexAdjacentTriangle.m_adjacentTriangles[j2];

		const uint32_t bSideVertexId            = vertexAdjacentTriangle.m_vertices[j1];
		const uint32_t bAdjacentTriangleId      = vertexAdjacentTriangle.m_adjacentTriangles[localOppositeVertexId];
		const uint32_t bOtherAdjacentTriangleId = currentTriangle.m_adjacentTriangles[i2];

		RemoveTriangle(aTriangleId);
		RemoveTriangle(bTriangleId);

		SetTriangle(aTriangleId, Triangle{
			{
				bVertexId,
				aSideVertexId,
				aVertexId
			},
			{
				aAdjacentTriangleId,
				aOtherAdjacentTriangleId,
				bTriangleId
			}
		});

		SetTriangle(bTriangleId, Triangle{
				{
					aVertexId,
					bSideVertexId,
					bVertexId
				},
				{
					bAdjacentTriangleId,
					bOtherAdjacentTriangleId,
					aTriangleId
				}
			}

		);

		for (const uint32_t i : currentTriangle.m_adjacentTriangles)
		{
			if (i == bTriangleId || i == Triangle::NULL_ID) { continue; }
			for (uint32_t& j : m_triangles[i].m_adjacentTriangles)
			{
				if (j == bTriangleId)
				{
					j = aTriangleId;
				}
			}
		}
		for (const uint32_t i : vertexAdjacentTriangle.m_adjacentTriangles)
		{
			if (i == aTriangleId || i == Triangle::NULL_ID)
			{
				continue;
			}
			for (uint32_t& j : m_triangles[i].m_adjacentTriangles)
			{
				if (j == aTriangleId)
				{
					j = bTriangleId;
				}
			}
		}
	}

	uint32_t NavMesh::FindInitialConstraintTriangle(const Edge& edge) const
	{
		for (const uint32_t triangleId : m_vertexToTriangleMap[edge.m_start])
		{
			uint32_t p1Id;
			uint32_t p2Id;

			const Triangle& triangle = m_triangles[triangleId];

			GetNextVerticesClockwise(triangle, edge.m_start, p1Id, p2Id);

			const Point3D& p1 = m_vertices[p1Id];
			const Point3D& p2 = m_vertices[p2Id];

			if (GetSegmentOrientation(m_vertices[edge.m_start], p1, p2) == 0)
			{
				continue;
			}
			if (Intersect2D(m_vertices[edge.m_start], m_vertices[edge.m_end], p1, p2))
			{
				return triangleId;
			}
		}

		for (const uint32_t triangleId : m_vertexToTriangleMap[edge.m_start])
		{
			uint32_t p1Id;
			uint32_t p2Id;

			const Triangle& triangle = m_triangles[triangleId];

			GetNextVerticesClockwise(triangle, edge.m_start, p1Id, p2Id);

			const Point3D& p1 = m_vertices[p1Id];
			const Point3D& p2 = m_vertices[p2Id];

			if (GetSegmentOrientation(m_vertices[edge.m_start], p1, p2) == 0)
			{
				uint32_t aTriangleId;
				uint32_t bTriangleId;
				GetEdgeTriangles({ p1Id, p2Id }, aTriangleId, bTriangleId);

				const Triangle& oppositeTriangle = m_triangles[aTriangleId != triangleId ? aTriangleId : bTriangleId];

				const uint32_t oppositeVertexId = FindOppositeSideVertexId(oppositeTriangle, triangle);

				if (oppositeVertexId == edge.m_end)
				{
					return triangleId;
				}

				const bool a = Intersect2D(m_vertices[edge.m_start], m_vertices[edge.m_end], p1, m_vertices[oppositeVertexId]);
				const bool b = Intersect2D(m_vertices[edge.m_start], m_vertices[edge.m_end], m_vertices[oppositeVertexId], p2);

				if (a || b)
				{
					return triangleId;
				}
			}
		}

		assert(false);

		return 0xFFFFFFFF;
	}

	uint32_t NavMesh::FindInitialConstraintTriangle(const uint32_t startVertex, const Point3D& endPoint, const uint32_t endPointTriangle) const
	{
		for (const uint32_t triangleId : m_vertexToTriangleMap[startVertex])
		{
			if (triangleId == endPointTriangle)
			{
				return triangleId;
			}

			uint32_t p1Id;
			uint32_t p2Id;

			const Triangle& triangle = m_triangles[triangleId];

			GetNextVerticesClockwise(triangle, startVertex, p1Id, p2Id);

			const Point3D& p1 = m_vertices[p1Id];
			const Point3D& p2 = m_vertices[p2Id];

			if (GetSegmentOrientation(m_vertices[startVertex], p1, p2) == 0)
			{
				continue;
			}
			if (Intersect2D(m_vertices[startVertex], endPoint, p1, p2))
			{
				return triangleId;
			}
		}

		for (const uint32_t triangleId : m_vertexToTriangleMap[startVertex])
		{
			uint32_t p1Id;
			uint32_t p2Id;

			const Triangle& triangle = m_triangles[triangleId];

			GetNextVerticesClockwise(triangle, startVertex, p1Id, p2Id);

			const Point3D& p1 = m_vertices[p1Id];
			const Point3D& p2 = m_vertices[p2Id];

			if (GetSegmentOrientation(m_vertices[startVertex], p1, p2) == 0)
			{
				uint32_t aTriangleId;
				uint32_t bTriangleId;
				GetEdgeTriangles({ p1Id, p2Id }, aTriangleId, bTriangleId);

				const Triangle& oppositeTriangle = m_triangles[aTriangleId != triangleId ? aTriangleId : bTriangleId];

				if (IsInsideTriangle(endPoint, m_vertices[oppositeTriangle.m_vertices[0]],
					m_vertices[oppositeTriangle.m_vertices[1]], m_vertices[oppositeTriangle.m_vertices[2]]))
				{
					return triangleId;
				}

				const uint32_t oppositeVertexId = FindOppositeSideVertexId(oppositeTriangle, triangle);


				const bool a = Intersect2D(m_vertices[startVertex], endPoint, p1, m_vertices[oppositeVertexId]);
				const bool b = Intersect2D(m_vertices[startVertex], endPoint, m_vertices[oppositeVertexId], p2);

				if (a || b)
				{
					return triangleId;
				}
			}
		}

		assert(false);

		return 0xFFFFFFFF;
	}

	uint32_t NavMesh::FindInitialConstraintTriangle(const Point3D& startPoint, const Point3D& endPoint, const uint32_t endPointTriangle) const
	{
		const uint32_t triangleId = FindTriangleId(startPoint);

		{
			if (triangleId == endPointTriangle)
			{
				return triangleId;
			}

			const Triangle& triangle = m_triangles[triangleId];

			const uint32_t p1Id = triangle.m_vertices[0];
			const uint32_t p2Id = triangle.m_vertices[1];

			const Point3D& p1 = m_vertices[p1Id];
			const Point3D& p2 = m_vertices[p2Id];

			if (!GetSegmentOrientation(startPoint, p1, p2) == 0 &&
				Intersect2D(startPoint, endPoint, p1, p2))
			{
				return triangleId;
			}
		}

		{
			const Triangle& triangle = m_triangles[triangleId];

			const uint32_t p1Id = triangle.m_vertices[0];
			const uint32_t p2Id = triangle.m_vertices[1];

			const Point3D& p1 = m_vertices[p1Id];
			const Point3D& p2 = m_vertices[p2Id];

			if (GetSegmentOrientation(startPoint, p1, p2) == 0)
			{
				uint32_t aTriangleId;
				uint32_t bTriangleId;
				GetEdgeTriangles({ p1Id, p2Id }, aTriangleId, bTriangleId);

				const Triangle& oppositeTriangle = m_triangles[aTriangleId != triangleId ? aTriangleId : bTriangleId];

				if (IsInsideTriangle(endPoint, m_vertices[oppositeTriangle.m_vertices[0]],
					m_vertices[oppositeTriangle.m_vertices[1]], m_vertices[oppositeTriangle.m_vertices[2]]))
				{
					return triangleId;
				}

				const uint32_t oppositeVertexId = FindOppositeSideVertexId(oppositeTriangle, triangle);


				const bool a = Intersect2D(startPoint, endPoint, p1, m_vertices[oppositeVertexId]);
				const bool b = Intersect2D(startPoint, endPoint, m_vertices[oppositeVertexId], p2);

				if (a || b)
				{
					return triangleId;
				}
			}
		}

		assert(false);

		return 0xFFFFFFFF;
	}

	static bool IsAngleLessThan180(const Point3D& a, const Point3D& b, const Point3D& c)
	{
		Vector3D ab = a - c;
		Vector3D bc = c - b;

		return ab.x * bc.y - ab.y * bc.x >= 0.0f;
	}

	bool NavMesh::IsConvex(const Edge& edge) const
	{
		uint32_t aTriangleId;
		uint32_t bTriangleId;
		GetEdgeTriangles(edge, aTriangleId, bTriangleId);

		const Triangle& aTriangle = m_triangles[aTriangleId];
		const Triangle& bTriangle = m_triangles[bTriangleId];

		const uint32_t aVertexId = FindOppositeSideVertexId(bTriangle, aTriangle);
		const uint32_t bVertexId = FindOppositeSideVertexId(aTriangle, bTriangle);

		const Point3D& a = m_vertices[bVertexId];
		const Point3D& b = m_vertices[edge.m_start];
		const Point3D& c = m_vertices[aVertexId];
		const Point3D& d = m_vertices[edge.m_end];

		const bool aa = IsAngleLessThan180(a, b, c);
		const bool bb = IsAngleLessThan180(b, c, d);
		const bool cc = IsAngleLessThan180(c, d, a);
		const bool dd = IsAngleLessThan180(d, a, b);

		return aa && bb && cc && dd;
	}

	NavMesh::Edge NavMesh::FlipDiagonal(const Edge& edge)
	{
		uint32_t aTriangleId;
		uint32_t bTriangleId;
		GetEdgeTriangles(edge, aTriangleId, bTriangleId);

		const Triangle& aTriangle = m_triangles[aTriangleId];
		const Triangle& bTriangle = m_triangles[bTriangleId];

		const uint32_t aVertexId = FindOppositeSideVertexId(bTriangle, aTriangle);
		const uint32_t bVertexId = FindOppositeSideVertexId(aTriangle, bTriangle);

		FlipDiagonal(aTriangleId, bTriangleId);

		return Edge{ aVertexId, bVertexId };
	}

	bool NavMesh::DoesEdgeAlreadyExist(const Edge& edge, const bool mustBeTraversable) const
	{
		for (uint32_t triangleId : m_vertexToTriangleMap[edge.m_start])
		{
			if (m_vertexToTriangleMap[edge.m_end].find(triangleId) != m_vertexToTriangleMap[edge.m_end].end())
			{
				if (mustBeTraversable && !m_triangles[triangleId].m_isNavigable)
				{
					continue;
				}
				return true;
			}
		}

		return false;
	}

	std::queue<NavMesh::Edge> NavMesh::GetIntersectingEdges(const Edge& edge) const
	{
		uint32_t currentTriangleId      = FindInitialConstraintTriangle(edge);
		const Triangle* currentTriangle = &m_triangles[currentTriangleId];
		uint32_t p1Id;
		uint32_t p2Id;
		GetNextVerticesClockwise(*currentTriangle, edge.m_start, p1Id, p2Id);

		std::queue<Edge> intersectingEdges;
		uint32_t nextTriangleId = p1Id == currentTriangle->m_vertices[0] ?
									  currentTriangle->m_adjacentTriangles[0] :
									  p1Id == currentTriangle->m_vertices[1] ?
										  currentTriangle->m_adjacentTriangles[1] :
										  currentTriangle->m_adjacentTriangles[2];

		Edge currentEdge = intersectingEdges.emplace(Edge{ p1Id, p2Id });

		while (true)
		{
			const uint32_t oppositeVertexId = FindOppositeSideVertexId(m_triangles[nextTriangleId], *currentTriangle);

			if (oppositeVertexId == edge.m_end)
			{
				return intersectingEdges;
			}

			currentTriangleId = nextTriangleId;
			currentTriangle   = &m_triangles[currentTriangleId];

			if (Intersect2D(m_vertices[edge.m_start], m_vertices[edge.m_end], m_vertices[currentEdge.m_start],
				m_vertices[oppositeVertexId]))
			{
				nextTriangleId = currentEdge.m_start == currentTriangle->m_vertices[0] ?
									 currentTriangle->m_adjacentTriangles[0] :
									 currentEdge.m_start == currentTriangle->m_vertices[1] ?
										 currentTriangle->m_adjacentTriangles[1] :
										 currentTriangle->m_adjacentTriangles[2];

				currentEdge = intersectingEdges.emplace(Edge{ currentEdge.m_start, oppositeVertexId });
			}
			else
			{
				nextTriangleId = oppositeVertexId == currentTriangle->m_vertices[0] ?
									 currentTriangle->m_adjacentTriangles[0] :
									 oppositeVertexId == currentTriangle->m_vertices[1] ?
										 currentTriangle->m_adjacentTriangles[1] :
										 currentTriangle->m_adjacentTriangles[2];

				currentEdge = intersectingEdges.emplace(Edge{ oppositeVertexId, currentEdge.m_end });
			}
		}
	}

	void NavMesh::AddConstraint(const Edge& edge)
	{
		if (DoesEdgeAlreadyExist(edge))
		{
			return;
		}

		std::queue<Edge> intersectingEdges = GetIntersectingEdges(edge);

		std::queue<Edge> newlyCreatedEdges;

		while (!intersectingEdges.empty())
		{
			const Edge& currentEdge = intersectingEdges.front();
			intersectingEdges.pop();

			if (IsConvex(currentEdge))
			{
				const Edge newTrianglePair = FlipDiagonal(currentEdge);

				if (edge.m_start != newTrianglePair.m_start && edge.m_end != newTrianglePair.m_start &&
					edge.m_start != newTrianglePair.m_end && edge.m_end != newTrianglePair.m_end &&
					Intersect2D(m_vertices[newTrianglePair.m_start], m_vertices[newTrianglePair.m_end],
						m_vertices[edge.m_start], m_vertices[edge.m_end]))
				{
					intersectingEdges.emplace(newTrianglePair);
				}
				else
				{
					newlyCreatedEdges.emplace(newTrianglePair);
				}
			}
			else
			{
				intersectingEdges.emplace(currentEdge);
			}
		}

		while (!newlyCreatedEdges.size())
		{
			const Edge& currentEdge = newlyCreatedEdges.front();
			newlyCreatedEdges.pop();

			uint32_t aTriangleId;
			uint32_t bTriangleId;
			GetEdgeTriangles(currentEdge, aTriangleId, bTriangleId);

			const Triangle& aTriangle = m_triangles[aTriangleId];
			const Triangle& bTriangle = m_triangles[bTriangleId];

			const uint32_t vertexId         = FindOppositeSideVertexId(bTriangle, aTriangle);
			const uint32_t oppositeVertexId = FindOppositeSideVertexId(aTriangle, bTriangle);

			if (TriangleFlipTest(vertexId, aTriangle, oppositeVertexId))
			{
				FlipDiagonal(aTriangleId, bTriangleId, vertexId, oppositeVertexId);

				const Edge newEdge = Edge{ vertexId, oppositeVertexId };

				newlyCreatedEdges.emplace(newEdge);
			}
		}
	}

	void NavMesh::AddObstacle(const std::vector<Point3D>& vertices)
	{
		const uint32_t startIndex = (uint32_t) m_vertices.size();

		for (const Point3D& vertex : vertices)
		{
			AddVertex(vertex);
		}

		for (uint32_t i = 0; i < vertices.size() + 1; ++i)
		{
			const uint32_t edgeStart = startIndex + i;
			const uint32_t edgeEnd   = i == vertices.size() - 1 ? startIndex : edgeStart + 1;
			AddConstraint(Edge{ edgeStart, edgeEnd });
		}
	}

	Point3D NavMesh::GetVertexPosition(const uint32_t vertexId) const
	{
		return m_vertices[vertexId];
	}

	bool NavMesh::DoesDirectPathExists(const uint32_t startVertexId, const uint32_t endVertexId) const
	{
		const Edge edge = { startVertexId, endVertexId };

		if (DoesEdgeAlreadyExist(edge, true))
		{
			return true;
		}

		uint32_t currentTriangleId      = FindInitialConstraintTriangle(edge);
		const Triangle* currentTriangle = &m_triangles[currentTriangleId];
		uint32_t p1Id;
		uint32_t p2Id;
		GetNextVerticesClockwise(*currentTriangle, edge.m_start, p1Id, p2Id);

		uint32_t nextTriangleId = p1Id == currentTriangle->m_vertices[0] ?
									  currentTriangle->m_adjacentTriangles[0] :
									  p1Id == currentTriangle->m_vertices[1] ?
										  currentTriangle->m_adjacentTriangles[1] :
										  currentTriangle->m_adjacentTriangles[2];

		uint32_t previousVertex = p1Id;

		while (true)
		{
			if (!currentTriangle->m_isNavigable)
			{
				return false;
			}

			const uint32_t oppositeVertexId = FindOppositeSideVertexId(m_triangles[nextTriangleId], *currentTriangle);

			if (oppositeVertexId == edge.m_end)
			{
				return true;
			}

			currentTriangleId = nextTriangleId;
			currentTriangle   = &m_triangles[currentTriangleId];

			if (Intersect2D(m_vertices[edge.m_start], m_vertices[edge.m_end], m_vertices[previousVertex],
				m_vertices[oppositeVertexId]))
			{
				nextTriangleId = previousVertex == currentTriangle->m_vertices[0] ?
									 currentTriangle->m_adjacentTriangles[0] :
									 previousVertex == currentTriangle->m_vertices[1] ?
										 currentTriangle->m_adjacentTriangles[1] :
										 currentTriangle->m_adjacentTriangles[2];
			}
			else
			{
				nextTriangleId = oppositeVertexId == currentTriangle->m_vertices[0] ?
									 currentTriangle->m_adjacentTriangles[0] :
									 oppositeVertexId == currentTriangle->m_vertices[1] ?
										 currentTriangle->m_adjacentTriangles[1] :
										 currentTriangle->m_adjacentTriangles[2];


				previousVertex = oppositeVertexId;
			}
		}
	}

	bool NavMesh::DoesDirectPathExists(const uint32_t startVertexId, const Point3D& endPoint) const
	{
		const uint32_t targetTriangleId = FindTriangleId(endPoint);

		uint32_t currentTriangleId      = FindInitialConstraintTriangle(startVertexId, endPoint, targetTriangleId);
		const Triangle* currentTriangle = &m_triangles[currentTriangleId];
		uint32_t p1Id;
		uint32_t p2Id;
		GetNextVerticesClockwise(*currentTriangle, startVertexId, p1Id, p2Id);

		uint32_t nextTriangleId = p1Id == currentTriangle->m_vertices[0] ?
									  currentTriangle->m_adjacentTriangles[0] :
									  p1Id == currentTriangle->m_vertices[1] ?
										  currentTriangle->m_adjacentTriangles[1] :
										  currentTriangle->m_adjacentTriangles[2];

		uint32_t previousVertex = p1Id;

		while (true)
		{
			if (!currentTriangle->m_isNavigable)
			{
				return false;
			}

			if (nextTriangleId == targetTriangleId)
			{
				return true;
			}

			const uint32_t oppositeVertexId = FindOppositeSideVertexId(m_triangles[nextTriangleId], *currentTriangle);


			currentTriangleId = nextTriangleId;
			currentTriangle   = &m_triangles[currentTriangleId];

			if (Intersect2D(m_vertices[startVertexId], endPoint, m_vertices[previousVertex],
				m_vertices[oppositeVertexId]))
			{
				nextTriangleId = previousVertex == currentTriangle->m_vertices[0] ?
									 currentTriangle->m_adjacentTriangles[0] :
									 previousVertex == currentTriangle->m_vertices[1] ?
										 currentTriangle->m_adjacentTriangles[1] :
										 currentTriangle->m_adjacentTriangles[2];
			}
			else
			{
				nextTriangleId = oppositeVertexId == currentTriangle->m_vertices[0] ?
									 currentTriangle->m_adjacentTriangles[0] :
									 oppositeVertexId == currentTriangle->m_vertices[1] ?
										 currentTriangle->m_adjacentTriangles[1] :
										 currentTriangle->m_adjacentTriangles[2];


				previousVertex = oppositeVertexId;
			}
		}
	}

	bool NavMesh::DoesDirectPathExists(const Point3D& startPoint, const Point3D& endPoint) const
	{
		const uint32_t targetTriangleId = FindTriangleId(endPoint);
		uint32_t currentTriangleId      = FindTriangleId(startPoint); // FindInitialConstraintTriangle(startPoint, endPoint, targetTriangleId);

		const Triangle* currentTriangle = &m_triangles[currentTriangleId];

		if (targetTriangleId == currentTriangleId)
		{
			return currentTriangle->m_isNavigable;
		}

		uint32_t nextTriangleId = currentTriangleId;
		uint32_t previousVertex = 0xFFFFFFFF;

		for (uint32_t i = 0; i < 3; ++i)
		{
			const Point3D& a = m_vertices[currentTriangle->m_vertices[i]];
			const Point3D& b = m_vertices[currentTriangle->m_vertices[(i + 1) % 3]];
			if (Intersect2D(startPoint, endPoint, a, b))
			{
				previousVertex = currentTriangle->m_vertices[i];
				nextTriangleId = currentTriangle->m_adjacentTriangles[i];

				//if points are collinear, look for another intersecting edge
				if (GetSegmentOrientation(a, b, startPoint) != 0)
				{
					break;
				}
			}
		}

		if (previousVertex == 0xFFFFFFFF)
		{
			__debugbreak();
		}

		while (true)
		{
			if (!currentTriangle->m_isNavigable)
			{
				return false;
			}

			if (nextTriangleId == targetTriangleId)
			{
				return true;
			}

			const uint32_t oppositeVertexId = FindOppositeSideVertexId(m_triangles[nextTriangleId], *currentTriangle);


			currentTriangleId = nextTriangleId;
			currentTriangle   = &m_triangles[currentTriangleId];

			if (Intersect2D(startPoint, endPoint, m_vertices[previousVertex],
				m_vertices[oppositeVertexId]))
			{
				nextTriangleId = previousVertex == currentTriangle->m_vertices[0] ?
									 currentTriangle->m_adjacentTriangles[0] :
									 previousVertex == currentTriangle->m_vertices[1] ?
										 currentTriangle->m_adjacentTriangles[1] :
										 currentTriangle->m_adjacentTriangles[2];
			}
			else
			{
				nextTriangleId = oppositeVertexId == currentTriangle->m_vertices[0] ?
									 currentTriangle->m_adjacentTriangles[0] :
									 oppositeVertexId == currentTriangle->m_vertices[1] ?
										 currentTriangle->m_adjacentTriangles[1] :
										 currentTriangle->m_adjacentTriangles[2];


				previousVertex = oppositeVertexId;
			}
		}
	}
}
