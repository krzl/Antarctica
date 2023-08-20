#pragma once

struct Submesh;

namespace Navigation
{
	class Terrain;

	class NavMesh
	{
		struct Node;

		struct Triangle
		{
			static constexpr uint32_t NULL_ID = 0xFFFFFFFF;

			std::array<uint32_t, 3> m_vertices          = { NULL_ID, NULL_ID, NULL_ID };
			std::array<uint32_t, 3> m_adjacentTriangles = { NULL_ID, NULL_ID, NULL_ID };
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

		std::vector<uint32_t> m_traversableIndices;
		std::vector<uint32_t> m_nonTraversableIndices;

	private:

		void SetTriangle(uint32_t triangleId, const Triangle& triangle);
		void RemoveTriangle(uint32_t triangleId);

		uint32_t FindTriangleId(const Point3D& vertex) const;

		bool TriangleFlipTest(uint32_t vertexId, const Triangle& triangle, uint32_t oppositeVertexId) const;
		bool IsConvex(const Edge& edge) const;

		bool DoesEdgeAlreadyExist(const Edge& edge);
		std::queue<Edge> GetIntersectingEdges(const Edge& edge) const;

		void GetEdgeTriangles(const Edge& edge, uint32_t& aTriangleId, uint32_t& bTriangleId) const;

		void FlipDiagonal(uint32_t aTriangleId, uint32_t bTriangleId, uint32_t aVertexId = Triangle::NULL_ID, uint32_t bVertexId = Triangle::NULL_ID);
		Edge FlipDiagonal(const Edge& edge);

		static uint32_t FindOppositeSideVertexId(const Triangle& searchTriangle, const Triangle& compareTriangle);
		static uint32_t GetAdjacentTriangleId(const std::vector<unsigned>& adjacentTriangles, const std::vector<unsigned>::iterator& it,
											  bool seekLeft);

		static void GetNextVerticesClockwise(const Triangle& triangle, uint32_t startingVertexId, uint32_t& p1, uint32_t& p2);

		uint32_t FindInitialConstraintTriangle(const Edge& edge) const;


		std::vector<Point3D> m_vertices;
		std::vector<std::unordered_set<uint32_t>> m_vertexToTriangleMap;
		std::vector<Triangle> m_triangles;
		std::vector<Edge> m_edges;
	};
}
