#include "stdafx.h"
#include "NavMesh.h"

#include "Assets/SubmeshData.h"

namespace Navigation
{
	void NavMesh::Node::AddConnection(Connection&& connection)
	{
		for (uint32_t i = 0; i < sizeof(m_connections) / sizeof(Connection); ++i)
		{
			if (m_connections[i].m_otherNodeId == Connection::UNINITIALIZED_NODE_ID)
			{
				m_connections[i] = connection;
				return;
			}
		}

		assert(false);
	}

	uint32_t NavMesh::GetNodeId(std::map<NodeLookupKey, uint32_t>& lookup, const std::vector<Point3D>& vertices,
								const uint32_t indexA, const uint32_t indexB)
	{
		NodeLookupKey key;
		key.m_a = Min(indexA, indexB);
		key.m_b = Max(indexA, indexB);

		const auto it = lookup.find(key);
		if (it != lookup.end())
		{
			return it->second;
		}

		const uint32_t nodeId = static_cast<uint32_t>(m_nodes.size());

		m_nodes.emplace_back(Node{
			(vertices[indexA] + vertices[indexB]) / 2.0f,
			vertices[indexA],
			vertices[indexB],
		});

		lookup.insert({ key, (uint32_t) m_nodes.size() });

		return nodeId;
	}

	void NavMesh::AddNodes(const std::vector<Point3D>& vertices, const std::vector<uint32_t>& indices, std::map<NodeLookupKey, uint32_t>& nodeLookup,
						   const ConnectionType connectionType, const uint32_t indexOffset)
	{
		for (uint32_t i = indexOffset; i < (indexOffset + indices.size()) / 3; ++i)
		{
			m_triangles[i].m_points[0] = vertices[indices[(i - indexOffset) * 3 + 0]];
			m_triangles[i].m_points[1] = vertices[indices[(i - indexOffset) * 3 + 1]];
			m_triangles[i].m_points[2] = vertices[indices[(i - indexOffset) * 3 + 2]];

			m_triangles[i].m_nodeIds[0] = GetNodeId(nodeLookup, vertices, indices[(i - indexOffset) * 3 + 0], indices[(i - indexOffset) * 3 + 1]);
			m_triangles[i].m_nodeIds[1] = GetNodeId(nodeLookup, vertices, indices[(i - indexOffset) * 3 + 1], indices[(i - indexOffset) * 3 + 2]);
			m_triangles[i].m_nodeIds[2] = GetNodeId(nodeLookup, vertices, indices[(i - indexOffset) * 3 + 0], indices[(i - indexOffset) * 3 + 2]);

			for (uint32_t j = 0; j < 3; ++j)
			{
				Node& currentNode = m_nodes[m_triangles[i].m_nodeIds[j]];
				for (uint32_t k = 0; k < 3; ++k)
				{
					if (j == k)
					{
						continue;
					}
					Node& otherNode = m_nodes[m_triangles[i].m_nodeIds[k]];
					currentNode.AddConnection(Connection{
						m_triangles[i].m_nodeIds[k],
						Magnitude(otherNode.m_position - currentNode.m_position),
						connectionType
					});
				}
			}
		}
	}

	NavMesh::NavMesh()
	{
		std::map<NodeLookupKey, uint32_t> nodeLookup;
/*
		m_triangles.resize(indices.size() / 3);

		//AddNodes(vertices, indices, nodeLookup, ConnectionType::WALKABLE, 0);
		//AddNodes(vertices, nonTraversableIndices, nodeLookup, ConnectionType::NOT_WALKABLE, (uint32_t) indices.size());

		m_vertices              = vertices;
		m_traversableIndices    = indices;
		m_nonTraversableIndices = nonTraversableIndices;*/
	}
}
