#pragma once

namespace Navigation
{
	class Terrain;
	class NavMesh;

	class PathFinding
	{
		friend class GameState;
		friend class ArriveBehavior;
		friend class MovementSystem;

		enum class NodeState
		{
			CLOSED,
			OPEN,
			END_NODE,
			UNVISITED
		};

		struct NodeRecord
		{
			float m_currentCost        = 0.0f;
			float m_estimatedTotalCost = 0.0f;
			int16_t m_previousVertexId = -1;
			NodeState m_state          = NodeState::UNVISITED;
		};

		struct NodeRecordComp
		{
			bool operator()(const NodeRecord* a, const NodeRecord* b) const;
		};

	public:

		static std::optional<std::list<uint32_t>> FindPath(const Point3D& start, const Point3D& end);

	private:

		static NodeRecord* ProcessPath(const Point3D& start, const Point3D& end, const uint32_t startTriangleId,
									   std::vector<NodeRecord>& nodeRecords);

		static uint32_t GetClosestDirectPointOnPath(const Point3D& pathStart, const NodeRecord* currentPathEndNode,
													const std::vector<NodeRecord>& nodeRecords, uint32_t vertexId);

		static uint32_t GetClosestDirectPointOnPath(const Point3D& pathStart, const NodeRecord* currentPathEndNode,
													const std::vector<NodeRecord>& nodeRecords, const Point3D& pathEnd);

		static float EstimateTotalCost(const Point3D& start, const Point3D& end);

		static NavMesh* m_navMesh;
		static Terrain* m_terrain;
	};
}
