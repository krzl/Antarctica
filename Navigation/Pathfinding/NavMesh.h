#pragma once
#include "Components/MovementComponent.h"

struct Submesh;

namespace Navigation
{
	class Terrain;

	class NavMesh
	{
		friend class PathFinding;

		struct Node;

		struct Triangle
		{
			static constexpr uint32_t NULL_ID = 0xFFFFFFFF;

			std::array<uint32_t, 3> m_vertices          = { NULL_ID, NULL_ID, NULL_ID };
			std::array<uint32_t, 3> m_adjacentTriangles = { NULL_ID, NULL_ID, NULL_ID };
			bool m_isNavigable                          = false;
			uint32_t m_islandId                         = 0;
		};

	public:

		struct Edge
		{
			uint32_t m_start;
			uint32_t m_end;
		};

		explicit NavMesh(const std::vector<Point3D>& vertices, std::vector<Edge>&& edges, const Terrain& terrain);

		void AddVertex(const Point3D& vertex);
		void AddConstraint(const Edge& edge);

		void AddObstacle(const std::vector<Point3D>& vertices);

		Point3D GetVertexPosition(const uint32_t vertexId) const;

		bool DoesDirectPathExists(uint32_t startVertexId, uint32_t endVertexId) const;
		bool DoesDirectPathExists(uint32_t startVertexId, const Point3D& endPoint) const;
		bool DoesDirectPathExists(const Point3D& startPoint, const Point3D& endPoint) const;

		bool FindCollisionPoint(const Point3D& location, float radius, Point2D& outCollisionPoint, Vector2D& outCollisionNormal,
								float& outPenetration);

		bool FindCollisionPoint(const Point3D& location, const Vector2D& direction, float radius, Point2D& outCollisionPoint,
								Vector2D& outCollisionNormal, float& outPenetration);

		Point2D FindNearestPointOnNavMesh(const Point2D& location, Vector2D* normal = nullptr);

	private:

		void SetTriangle(uint32_t triangleId, const Triangle& triangle);
		void RemoveTriangle(uint32_t triangleId);

		uint32_t FindTriangleId(const Point3D& vertex) const;

		bool TriangleFlipTest(uint32_t vertexId, const Triangle& triangle, uint32_t oppositeVertexId) const;
		bool IsConvex(const Edge& edge) const;

		bool DoesEdgeAlreadyExist(const Edge& edge, bool mustBeTraversable = false) const;
		std::queue<Edge> GetIntersectingEdges(const Edge& edge) const;

		void GetEdgeTriangles(const Edge& edge, uint32_t& aTriangleId, uint32_t& bTriangleId) const;

		void FlipDiagonal(uint32_t aTriangleId, uint32_t bTriangleId, uint32_t aVertexId = Triangle::NULL_ID, uint32_t bVertexId = Triangle::NULL_ID);
		Edge FlipDiagonal(const Edge& edge);

		static uint32_t FindOppositeSideVertexId(const Triangle& searchTriangle, const Triangle& compareTriangle);
		static uint32_t GetAdjacentTriangleId(const std::vector<unsigned>& adjacentTriangles, const std::vector<unsigned>::iterator& it,
											  bool seekLeft);

		static void GetNextVerticesClockwise(const Triangle& triangle, uint32_t startingVertexId, uint32_t& p1, uint32_t& p2);

		uint32_t FindInitialConstraintTriangle(const Edge& edge) const;
		uint32_t FindInitialConstraintTriangle(const uint32_t startVertex, const Point3D& endPoint, const uint32_t endPointTriangle) const;

		void AssignIslandIds();
		void AssignIslandId(uint32_t triangleId, uint32_t islandId, std::set<uint32_t>& remainingTriangles);

		void FindCollisionPoint(const uint32_t triangleId, const Point3D& location, float radiusSqr, Point2D& collisionPoint,
								Vector2D& collisionNormal, float& distanceToPointSqr, std::set<uint32_t>& visitedTriangles);

		void FindCollisionPoint(const uint32_t triangleId, const Point3D& location, const Vector2D& direction, float radiusSqr,
								Point2D& collisionPoint, Vector2D& collisionNormal, float& distanceToPointSqr, std::set<uint32_t>& visitedTriangles);

		void FindNearestPointOnNavMesh(const uint32_t triangleId, const Point2D& location, Point2D& pointOnNavMesh, Vector2D* normal,
									   float& closestDistanceSqr, std::set<uint32_t>& visitedTriangles);

		std::vector<Point3D> m_vertices;
		std::vector<std::unordered_set<uint32_t>> m_vertexToTriangleMap;
		std::vector<Triangle> m_triangles;
		std::vector<Edge> m_edges;
	};
}
