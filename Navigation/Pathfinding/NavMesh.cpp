﻿#include "stdafx.h"
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

		AssignIslandIds();
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
		uint32_t currentTriangleId      = FindTriangleId(startPoint);

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
			//Fallback for handling points on triangle edge, find closest edge from point

			const Point3D& a = m_vertices[currentTriangle->m_vertices[0]];
			const Point3D& b = m_vertices[currentTriangle->m_vertices[1]];
			const Point3D& c = m_vertices[currentTriangle->m_vertices[2]];

			const Vector3D edgeA = Normalize(b - a);
			const Vector3D edgeB = Normalize(c - b);
			const Vector3D edgeC = Normalize(a - c);

			const float distA = GetDistanceSquaredFromLineToPoint(edgeA, startPoint);
			const float distB = GetDistanceSquaredFromLineToPoint(edgeB, startPoint);
			const float distC = GetDistanceSquaredFromLineToPoint(edgeC, startPoint);

			if (distA < distB)
			{
				if (distA < distC)
				{
					previousVertex = currentTriangle->m_vertices[0];
					nextTriangleId = currentTriangle->m_adjacentTriangles[0];
				}
				else
				{
					previousVertex = currentTriangle->m_vertices[2];
					nextTriangleId = currentTriangle->m_adjacentTriangles[2];
				}
			}
			else
			{
				if (distB < distC)
				{
					previousVertex = currentTriangle->m_vertices[1];
					nextTriangleId = currentTriangle->m_adjacentTriangles[1];
				}
				else
				{
					previousVertex = currentTriangle->m_vertices[2];
					nextTriangleId = currentTriangle->m_adjacentTriangles[2];
				}
			}
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

	void NavMesh::AssignIslandIds()
	{
		std::set<uint32_t> remainingTriangles;

		for (uint32_t i = 0; i < m_triangles.size(); ++i)
		{
			if (m_triangles[i].m_isNavigable)
			{
				remainingTriangles.emplace(i);
			}
		}

		uint32_t islandId = 0;
		while (!remainingTriangles.empty())
		{
			auto it                   = remainingTriangles.begin();
			const uint32_t triangleId = *it;
			remainingTriangles.erase(it);

			AssignIslandId(triangleId, islandId++, remainingTriangles);
		}
	}

	void NavMesh::AssignIslandId(const uint32_t triangleId, const uint32_t islandId, std::set<uint32_t>& remainingTriangles)
	{
		Triangle& triangle  = m_triangles[triangleId];
		triangle.m_islandId = islandId;

		for (uint32_t i = 0; i < 3; ++i)
		{
			if (const bool isRemaining = remainingTriangles.erase(triangle.m_adjacentTriangles[i]))
			{
				AssignIslandId(triangle.m_adjacentTriangles[i], islandId, remainingTriangles);
			}
		}
	}

	bool NavMesh::FindCollisionPoint(const Sphere& sphere, Point2D& outCollisionPoint, Vector2D& outCollisionNormal, float& outPenetration)
	{
		const uint32_t startTriangleId = FindTriangleId(sphere.m_center);

		std::set<uint32_t> visitedTriangles;
		visitedTriangles.emplace(startTriangleId);

		float distanceToPointSqr = 100000.0f;

		if (!m_triangles[startTriangleId].m_isNavigable)
		{
			FindNearestPointOnNavMesh(startTriangleId, (Point2D) sphere.m_center.xy, outCollisionPoint, &outCollisionNormal, distanceToPointSqr,
				visitedTriangles);
			outPenetration = Terathon::Sqrt(distanceToPointSqr);
			return true;
		}

		FindCollisionPoint(startTriangleId, sphere, sphere.m_radius * sphere.m_radius, outCollisionPoint, outCollisionNormal, distanceToPointSqr,
			visitedTriangles);

		if (distanceToPointSqr > sphere.m_radius * sphere.m_radius)
		{
			return false;
		}

		outPenetration = sphere.m_radius - Terathon::Sqrt(distanceToPointSqr);
		return true;
	}

	void NavMesh::FindCollisionPoint(const uint32_t triangleId, const Sphere& sphere, const float radiusSqr, Point2D& collisionPoint,
									 Vector2D& collisionNormal, float& distanceToPointSqr, std::set<uint32_t>& visitedTriangles)
	{
		const Triangle& triangle = m_triangles[triangleId];

		const Point3D points[] = {
			m_vertices[triangle.m_vertices[0]],
			m_vertices[triangle.m_vertices[1]],
			m_vertices[triangle.m_vertices[2]]
		};

		for (uint32_t i = 0; i < 3; ++i)
		{
			const uint32_t neighborTriangleId = triangle.m_adjacentTriangles[i];
			if (neighborTriangleId == 0xFFFFFFFF)
			{
				continue;
			}

			const Triangle& neighborTriangle = m_triangles[neighborTriangleId];

			if (visitedTriangles.find(neighborTriangleId) != visitedTriangles.end())
			{
				continue;
			}

			visitedTriangles.emplace(neighborTriangleId);

			const Point3D closestPoint = GetClosestPointFromLineSegmentToPoint((Point3D) points[i].xy, (Point3D) points[(i + 1) % 3].xy,
				(Point3D) sphere.m_center.xy);
			const float distanceToSegmentSqr = SquaredMag(closestPoint.xy - sphere.m_center.xy);

			if (distanceToSegmentSqr < distanceToPointSqr)
			{
				if (!neighborTriangle.m_isNavigable)
				{
					const Vector2D lineDir = Normalize(points[(i + 1) % 3].xy - points[i].xy);
					collisionPoint         = closestPoint.xy;
					collisionNormal        = Vector2D(lineDir.y, -lineDir.x);
					distanceToPointSqr     = distanceToSegmentSqr;

					if (Dot(collisionNormal, sphere.m_center.xy - closestPoint.xy) > 0.0f)
					{
						collisionNormal = -collisionNormal;
					}
				}
				else
				{
					FindCollisionPoint(neighborTriangleId, sphere, radiusSqr, collisionPoint, collisionNormal, distanceToPointSqr, visitedTriangles);
				}
			}
		}
	}

	Point2D NavMesh::FindNearestPointOnNavMesh(const Point2D& location, Vector2D* normal)
	{
		const uint32_t startTriangleId = FindTriangleId(location);

		std::set<uint32_t> visitedTriangles;
		visitedTriangles.emplace(startTriangleId);


		Point2D pointOnNavMesh;
		float closestDistanceSqr = 100000.0f;
		FindNearestPointOnNavMesh(startTriangleId, (Point2D) location.xy, pointOnNavMesh, normal, closestDistanceSqr, visitedTriangles);
		return pointOnNavMesh;
	}

	void NavMesh::FindNearestPointOnNavMesh(const uint32_t triangleId, const Point2D& location, Point2D& pointOnNavMesh, Vector2D* normal,
											float& closestDistanceSqr, std::set<uint32_t>& visitedTriangles)
	{
		const Triangle& triangle = m_triangles[triangleId];

		const Point3D points[] = {
			m_vertices[triangle.m_vertices[0]],
			m_vertices[triangle.m_vertices[1]],
			m_vertices[triangle.m_vertices[2]]
		};

		for (uint32_t i = 0; i < 3; ++i)
		{
			const uint32_t neighborTriangleId = triangle.m_adjacentTriangles[i];
			if (neighborTriangleId == 0xFFFFFFFF)
			{
				continue;
			}

			const Triangle& neighborTriangle = m_triangles[neighborTriangleId];

			if (visitedTriangles.find(neighborTriangleId) != visitedTriangles.end())
			{
				continue;
			}

			visitedTriangles.emplace(neighborTriangleId);

			const Point3D closestPoint = GetClosestPointFromLineSegmentToPoint((Point3D) points[i].xy, (Point3D) points[(i + 1) % 3].xy,
				(Point3D) location.xy);
			const float distanceToSegmentSqr = SquaredMag(closestPoint.xy - location.xy);

			if (distanceToSegmentSqr < closestDistanceSqr)
			{
				if (neighborTriangle.m_isNavigable)
				{
					pointOnNavMesh = closestPoint.xy;
					if (normal)
					{
						const Vector2D lineDir = Normalize(points[(i + 1) % 3].xy - points[i].xy);

						*normal = Vector2D(lineDir.y, -lineDir.x);

						if (Dot(*normal, location.xy - closestPoint.xy) > 0.0f)
						{
							*normal = -*normal;
						}
					}
					closestDistanceSqr = distanceToSegmentSqr;
				}
				else
				{
					FindNearestPointOnNavMesh(neighborTriangleId, location, pointOnNavMesh, normal, closestDistanceSqr, visitedTriangles);
				}
			}
		}
	}

	bool NavMesh::FindCollisionPoint(const Sphere& sphere, const Vector2D& direction, Point2D& outCollisionPoint,
									 Vector2D& outCollisionNormal, float& outPenetration)
	{
		const uint32_t startTriangleId = FindTriangleId(sphere.m_center);

		std::set<uint32_t> visitedTriangles;
		visitedTriangles.emplace(startTriangleId);

		float distanceToPointSqr = 100000.0f;

		if (!m_triangles[startTriangleId].m_isNavigable)
		{
			FindNearestPointOnNavMesh(startTriangleId, (Point2D) sphere.m_center.xy, outCollisionPoint, &outCollisionNormal, distanceToPointSqr,
				visitedTriangles);
			outPenetration = Terathon::Sqrt(distanceToPointSqr);
			return true;
		}

		FindCollisionPoint(startTriangleId, sphere, sphere.m_radius * sphere.m_radius, direction, outCollisionPoint, outCollisionNormal,
			distanceToPointSqr, visitedTriangles);
		if (distanceToPointSqr > sphere.m_radius * sphere.m_radius)
		{
			return false;
		}

		outPenetration = sphere.m_radius - Terathon::Sqrt(distanceToPointSqr);
		return true;
	}

	void NavMesh::FindCollisionPoint(const uint32_t triangleId, const Sphere& sphere, const float radiusSqr, const Vector2D& direction,
									 Point2D& collisionPoint, Vector2D& collisionNormal, float& distanceToPointSqr,
									 std::set<uint32_t>& visitedTriangles)
	{
		const Triangle& triangle = m_triangles[triangleId];

		const Point3D points[] = {
			m_vertices[triangle.m_vertices[0]],
			m_vertices[triangle.m_vertices[1]],
			m_vertices[triangle.m_vertices[2]]
		};

		if (!triangle.m_isNavigable)
		{
			__debugbreak();
		}

		for (uint32_t i = 0; i < 3; ++i)
		{
			const uint32_t neighborTriangleId = triangle.m_adjacentTriangles[i];
			if (neighborTriangleId == 0xFFFFFFFF)
			{
				continue;
			}

			const Triangle& neighborTriangle = m_triangles[neighborTriangleId];

			if (visitedTriangles.find(neighborTriangleId) != visitedTriangles.end())
			{
				continue;
			}

			visitedTriangles.emplace(neighborTriangleId);

			const Point3D closestPoint = GetClosestPointFromLineSegmentToPoint((Point3D) points[i].xy, (Point3D) points[(i + 1) % 3].xy,
				(Point3D) sphere.m_center.xy);
			const float distanceToSegmentSqr = SquaredMag(closestPoint.xy - sphere.m_center.xy);

			if (distanceToSegmentSqr < distanceToPointSqr)
			{
				if (!neighborTriangle.m_isNavigable)
				{
					if (Dot(closestPoint.xy - sphere.m_center.xy, direction) > 0.0f)
					{
						const Vector2D lineDir = Normalize(points[(i + 1) % 3].xy - points[i].xy);
						collisionPoint         = closestPoint.xy;
						collisionNormal        = Vector2D(lineDir.y, -lineDir.x);
						distanceToPointSqr     = distanceToSegmentSqr;

						if (Dot(collisionNormal, sphere.m_center.xy - closestPoint.xy) > 0.0f)
						{
							collisionNormal = -collisionNormal;
						}
					}
				}
				else
				{
					FindCollisionPoint(neighborTriangleId, sphere, radiusSqr, direction, collisionPoint, collisionNormal, distanceToPointSqr,
						visitedTriangles);
				}
			}
		}
	}

	static std::optional<Point2D> LineLineIntersection(const Point2D& a, const Point2D& b, const Point2D& c, const Point2D& d)
	{
		const double a1 = b.y - a.y;
		const double b1 = a.x - b.x;
		const double c1 = a1 * (a.x) + b1 * (a.y);

		const double a2 = d.y - c.y;
		const double b2 = c.x - d.x;
		const double c2 = a2 * (c.x) + b2 * (c.y);

		const double determinant = a1 * b2 - a2 * b1;

		if (determinant == 0)
		{
			return std::optional<Point2D>();
		}

		const double x = (b2 * c1 - b1 * c2) / determinant;
		const double y = (a1 * c2 - a2 * c1) / determinant;
		return Point2D(x, y);
	}

	bool NavMesh::FindCollisionPoint(const Point2D& start, const Vector2D& direction, float maxDistance, Point2D& outCollisionPoint,
									 Vector2D& outCollisionNormal)
	{
		const Point2D end = start + direction * maxDistance;

		const uint32_t targetTriangleId = FindTriangleId(end);
		uint32_t currentTriangleId      = FindTriangleId(start);

		const Triangle* currentTriangle = &m_triangles[currentTriangleId];

		if (!currentTriangle->m_isNavigable)
		{
			outCollisionPoint = start;
			outCollisionNormal = Vector2D(0.0f, 1.0f);

			return true;
		}

		if (targetTriangleId == currentTriangleId)
		{
			return false;
		}

		uint32_t nextTriangleId = currentTriangleId;
		uint32_t previousVertex = 0xFFFFFFFF;
		uint32_t adjacentIndex;

		for (uint32_t i = 0; i < 3; ++i)
		{
			const Point3D& a = m_vertices[currentTriangle->m_vertices[i]];
			const Point3D& b = m_vertices[currentTriangle->m_vertices[(i + 1) % 3]];
			if (Intersect2D(start, end, a, b))
			{
				previousVertex = currentTriangle->m_vertices[i];
				nextTriangleId = currentTriangle->m_adjacentTriangles[i];
				adjacentIndex  = i;

				//if points are collinear, look for another intersecting edge
				if (GetSegmentOrientation(a, b, start) != 0)
				{
					break;
				}
			}
		}

		if (previousVertex == 0xFFFFFFFF)
		{
			//Fallback for handling points on triangle edge, find closest edge from point

			const Point3D& a = m_vertices[currentTriangle->m_vertices[0]];
			const Point3D& b = m_vertices[currentTriangle->m_vertices[1]];
			const Point3D& c = m_vertices[currentTriangle->m_vertices[2]];

			const Vector3D edgeA = Normalize(b - a);
			const Vector3D edgeB = Normalize(c - b);
			const Vector3D edgeC = Normalize(a - c);

			const float distA = GetDistanceSquaredFromLineToPoint(edgeA, start);
			const float distB = GetDistanceSquaredFromLineToPoint(edgeB, start);
			const float distC = GetDistanceSquaredFromLineToPoint(edgeC, start);

			if (distA < distB)
			{
				if (distA < distC)
				{
					adjacentIndex = 0;
				}
				else
				{
					adjacentIndex = 2;
				}
			}
			else
			{
				if (distB < distC)
				{
					adjacentIndex = 1;
				}
				else
				{
					adjacentIndex = 2;
				}
			}

			previousVertex = currentTriangle->m_vertices[adjacentIndex];
			nextTriangleId = currentTriangle->m_adjacentTriangles[adjacentIndex];
		}

		while (true)
		{
			if (!currentTriangle->m_isNavigable)
			{
				bool hasCollided     = false;
				float maxDistanceSqr = maxDistance * maxDistance;

				for (uint32_t i = 0; i < 3; ++i)
				{
					const Point2D a = (Point2D) m_vertices[m_triangles[currentTriangleId].m_vertices[(i + 0) % 3]].xy;
					const Point2D b = (Point2D) m_vertices[m_triangles[currentTriangleId].m_vertices[(i + 1) % 3]].xy;

					const std::optional<Point2D> collisionPoint = LineLineIntersection(a, b, start, end);

					if (!collisionPoint.has_value())
					{
						continue;
					}

					const float distanceSqr = SquaredMag(collisionPoint.value() - start);
					if (distanceSqr <= maxDistanceSqr)
					{
						maxDistanceSqr    = distanceSqr;
						outCollisionPoint = collisionPoint.value();

						const Vector2D lineDir = Normalize(b - a);
						outCollisionNormal     = Vector2D(lineDir.y, -lineDir.x);

						if (Dot(outCollisionNormal, direction) > 0.0f)
						{
							outCollisionNormal = -outCollisionNormal;
						}

						hasCollided = true;
					}
				}

				return hasCollided;
			}

			if (currentTriangleId == targetTriangleId)
			{
				return false;
			}

			const uint32_t oppositeVertexId = FindOppositeSideVertexId(m_triangles[nextTriangleId], *currentTriangle);

			currentTriangleId = nextTriangleId;
			currentTriangle   = &m_triangles[currentTriangleId];

			if (Intersect2D(start, end, m_vertices[previousVertex], m_vertices[oppositeVertexId]))
			{
				adjacentIndex = previousVertex == currentTriangle->m_vertices[0] ? 0 : previousVertex == currentTriangle->m_vertices[1] ? 1 : 2;
			}
			else
			{
				adjacentIndex  = oppositeVertexId == currentTriangle->m_vertices[0] ? 0 : oppositeVertexId == currentTriangle->m_vertices[1] ? 1 : 2;
				previousVertex = oppositeVertexId;
			}

			nextTriangleId = currentTriangle->m_adjacentTriangles[adjacentIndex];
		}
	}



	bool NavMesh::FindCollisionPoint(const Point2D& start, const Point2D& end, Point2D& outCollisionPoint, Vector2D& outCollisionNormal)
	{
		Vector2D direction    = end - start;
		const float magnitude = Magnitude(direction);
		direction /= magnitude;

		return FindCollisionPoint(start, direction, magnitude, outCollisionPoint, outCollisionNormal);
	}
}
