#pragma once

struct Submesh;

namespace Navigation
{
	class NavMesh
	{
		struct Node;

		enum class ConnectionType
		{
			NOT_WALKABLE,
			WALKABLE,
		};

		struct Connection
		{
			uint32_t m_otherNodeId = UNINITIALIZED_NODE_ID;
			float m_cost;
			ConnectionType m_type;

			static constexpr uint32_t UNINITIALIZED_NODE_ID = 0xFFFFFFFF;
		};

		struct Node
		{
			Point3D m_position;
			Point3D m_edges[2];
			Connection m_connections[4];

			void AddConnection(Connection&& connection);
		};

		struct Triangle
		{
			Point3D m_points[3];
			uint32_t m_nodeIds[3];
		};

		union NodeLookupKey
		{
			// ReSharper disable once CppInconsistentNaming
			friend std::size_t hash_value(const NodeLookupKey& obj)
			{
				std::size_t seed = 0x7A0D0D9A;
				seed ^= (seed << 6) + (seed >> 2) + 0x6FB2E052 + static_cast<std::size_t>(obj.m_key);
				return seed;
			}

			

			friend bool operator<(const NodeLookupKey& lhs, const NodeLookupKey& rhs) { return lhs.m_key < rhs.m_key; }
			friend bool operator<=(const NodeLookupKey& lhs, const NodeLookupKey& rhs) { return !(rhs < lhs); }
			friend bool operator>(const NodeLookupKey& lhs, const NodeLookupKey& rhs) { return rhs < lhs; }
			friend bool operator>=(const NodeLookupKey& lhs, const NodeLookupKey& rhs) { return !(lhs < rhs); }

			struct
			{
				uint32_t m_a;
				uint32_t m_b;
			};

			uint64_t m_key;
		};

	public:

		explicit NavMesh();

		//FOR VISUALIZATION ONLY
		std::vector<Point3D> m_vertices;
		std::vector<uint32_t> m_traversableIndices;
		std::vector<uint32_t> m_nonTraversableIndices;

	private:

		void AddNodes(const std::vector<Point3D>& vertices, const std::vector<uint32_t>& indices,
					  std::map<NodeLookupKey, uint32_t>& nodeLookup,
					  const ConnectionType connectionType, const uint32_t indexOffset);

		uint32_t GetNodeId(std::map<NodeLookupKey, uint32_t>& lookup, const std::vector<Point3D>& vertices,
						   const uint32_t indexA, const uint32_t indexB);

		std::vector<Node> m_nodes;
		std::vector<Triangle> m_triangles;
	};
}
