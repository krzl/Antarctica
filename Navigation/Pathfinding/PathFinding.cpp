#include "stdafx.h"
#include "PathFinding.h"

#include "NavMesh.h"

namespace Navigation
{
	NavMesh* PathFinding::m_navMesh;
	Terrain* PathFinding::m_terrain;

	bool PathFinding::NodeRecordComp::operator()(const NodeRecord* a, const NodeRecord* b) const
	{
		if (a == nullptr)
			return a;
		if (b == nullptr)
			return b;
		return a->m_estimatedTotalCost > b->m_estimatedTotalCost;
	}

	std::optional<std::list<Point2D>> PathFinding::FindPath(const Point3D& start, const Point3D& end, const float extrusion)
	{
		if (m_navMesh->DoesDirectPathExists(start, end))
		{
			return std::list<Point2D>();
		}

		const uint32_t startTriangleId = m_navMesh->FindTriangleId(start);
		const uint32_t endTriangleId   = m_navMesh->FindTriangleId(end);

		const NavMesh::Triangle& startTriangle = m_navMesh->m_triangles[startTriangleId];
		const NavMesh::Triangle& endTriangle   = m_navMesh->m_triangles[endTriangleId];

		if (!startTriangle.m_isNavigable ||
			!endTriangle.m_isNavigable ||
			startTriangle.m_islandId != endTriangle.m_islandId)
		{
			return std::optional<std::list<Point2D>>();
		}

		std::vector<NodeRecord> nodeRecords(m_navMesh->m_vertices.size());

		const NodeRecord* node = ProcessPath(start, end, startTriangleId, nodeRecords);

		if (node == nullptr)
		{
			return std::optional<std::list<Point2D>>();
		}

		const NodeRecord* lastNode = nullptr;

		std::list<Point2D> path;
		while (node != nullptr)
		{
			const Point2D previousPosition = (Point2D) (lastNode == nullptr ? end.xy : m_navMesh->m_vertices[lastNode - nodeRecords.data()].xy);
			const Point2D currentPosition  = (Point2D) m_navMesh->m_vertices[node - nodeRecords.data()].xy;

			lastNode = node;

			if (node->m_previousVertexId != -1)
			{
				node = &nodeRecords[node->m_previousVertexId];
			}
			else
			{
				node = nullptr;
			}

			const Point2D nextPosition = (Point2D) (node == nullptr ? start.xy : m_navMesh->m_vertices[node - nodeRecords.data()].xy);

			const Point2D extrusionPoint = (Point2D) GetClosestPointFromLineSegmentToPoint(previousPosition, nextPosition, currentPosition).xy;
			Vector2D extrusionDirection  = Normalize(currentPosition - extrusionPoint);

			if (IsNaN(extrusionDirection))
			{
				const Vector2D extrusionPerpendicular = Normalize(previousPosition - nextPosition);

				extrusionDirection = Vector2D(extrusionPerpendicular.y, -extrusionPerpendicular.x);

			}

			const Vector2D extrusionVector = extrusionDirection * extrusion;

			Point2D collisionPoint;
			Point2D collisionDirection;

			if (m_navMesh->FindCollisionPoint(currentPosition + extrusionDirection * 0.001f, currentPosition + extrusionVector, collisionPoint,
				collisionDirection))
			{
				const Point2D previousCollisionPoint = collisionPoint;
				if (m_navMesh->FindCollisionPoint(currentPosition - extrusionDirection * 0.001f, currentPosition - extrusionVector, collisionPoint,
					collisionDirection))
				{
					if (SquaredMag(previousCollisionPoint - currentPosition) > SquaredMag(collisionPoint - currentPosition))
					{
						collisionPoint = previousCollisionPoint;
					}

					path.emplace_front(collisionPoint);
				}
				else
				{
					path.emplace_front(currentPosition - extrusionVector);
				}
			}
			else
			{
				path.emplace_front(currentPosition + extrusionVector);
			}
		}

		return path;
	}

	PathFinding::NodeRecord* PathFinding::ProcessPath(const Point3D& start, const Point3D& end, const uint32_t startTriangleId,
													  std::vector<NodeRecord>& nodeRecords)
	{
		std::priority_queue<NodeRecord*, std::vector<NodeRecord*>, NodeRecordComp> priorityQueue;

		for (const unsigned vertexId : m_navMesh->m_triangles[startTriangleId].m_vertices)
		{
			nodeRecords[vertexId] = NodeRecord{
				0,
				EstimateTotalCost(m_navMesh->m_vertices[vertexId], end),
				-1,
				NodeState::OPEN
			};

			priorityQueue.emplace(&nodeRecords[vertexId]);
		}

		while (!priorityQueue.empty())
		{
			NodeRecord* current = priorityQueue.top();
			priorityQueue.pop();

			if (current->m_state == NodeState::END_NODE)
			{
				return current;
			}

			const uint32_t currentVertexId = current - nodeRecords.data();

			if (m_navMesh->DoesDirectPathExists(currentVertexId, end))
			{
				NodeRecord* startSegmentNode = &nodeRecords[currentVertexId];
				startSegmentNode->m_state    = NodeState::END_NODE;

				priorityQueue.emplace(startSegmentNode);

				continue;
			}

			for (const unsigned triangleId : m_navMesh->m_vertexToTriangleMap[currentVertexId])
			{
				if (!m_navMesh->m_triangles[triangleId].m_isNavigable)
				{
					continue;
				}
				for (const unsigned vertexId : m_navMesh->m_triangles[triangleId].m_vertices)
				{
					//TODO: check if same vertexId has been processed already in this loop
					if (vertexId == currentVertexId)
					{
						continue;
					}

					const uint32_t startSegmentVertexId = GetClosestDirectPointOnPath(start, current, nodeRecords, vertexId);
					const NodeRecord* startSegmentNode  = nullptr;
					Point3D startSegmentVertex;

					if (startSegmentVertexId != 0xFFFFFFFF)
					{
						startSegmentNode   = &nodeRecords[startSegmentVertexId];
						startSegmentVertex = m_navMesh->m_vertices[startSegmentVertexId];
					}
					else
					{
						startSegmentVertex = start;
					}

					const Point3D& endVertex  = m_navMesh->m_vertices[vertexId];
					const float endVertexCost = (startSegmentNode != nullptr ? startSegmentNode->m_currentCost : 0.0f) +
						Magnitude(endVertex.xy - startSegmentVertex.xy);


					NodeRecord& endNode = nodeRecords[vertexId];
					float endVertexHeuristic;

					if (endNode.m_state != NodeState::UNVISITED)
					{
						if (endNode.m_currentCost <= endVertexCost)
						{
							continue;
						}

						endVertexHeuristic = endNode.m_estimatedTotalCost - endNode.m_currentCost;
					}
					else
					{
						endVertexHeuristic = EstimateTotalCost(endVertex, end);
					}

					endNode.m_currentCost        = endVertexCost;
					endNode.m_estimatedTotalCost = endVertexCost + endVertexHeuristic;
					endNode.m_previousVertexId   = startSegmentVertexId;

					if (endNode.m_state != NodeState::OPEN)
					{
						priorityQueue.emplace(&endNode);
						endNode.m_state = NodeState::OPEN;
					}
				}
			}

			current->m_state = NodeState::CLOSED;
		}


		return nullptr;
	}

	uint32_t PathFinding::GetClosestDirectPointOnPath(const Point3D& pathStart, const NodeRecord* currentPathEndNode,
													  const std::vector<NodeRecord>& nodeRecords, const uint32_t vertexId)
	{
		const NodeRecord* bestNode = currentPathEndNode;

		while (true)
		{
			if (currentPathEndNode->m_previousVertexId == -1)
			{
				if (m_navMesh->DoesDirectPathExists(vertexId, pathStart))
				{
					return -1;
				}
				return bestNode - nodeRecords.data();
			}
			else
			{
				if (m_navMesh->DoesDirectPathExists(vertexId, currentPathEndNode->m_previousVertexId))
				{
					bestNode = &nodeRecords[currentPathEndNode->m_previousVertexId];
				}
				currentPathEndNode = &nodeRecords[currentPathEndNode->m_previousVertexId];
			}
		}
	}

	float PathFinding::EstimateTotalCost(const Point3D& start, const Point3D& end)
	{
		return Magnitude(end.xy - start.xy);
	}
}
