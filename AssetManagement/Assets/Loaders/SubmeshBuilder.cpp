#include "stdafx.h"
#include "SubmeshBuilder.h"

SubmeshBuilder::SubmeshBuilder(std::string&& name, std::vector<Point3D>&& positions, std::vector<uint32_t>& indices) :
	m_name(std::move(name)),
	m_indices(reinterpret_cast<uint8_t*>(indices.data()),
		reinterpret_cast<uint8_t*>(indices.data()) + indices.size() * 4),
	m_positions(std::move(positions)),
	m_skeleton() {}

void SubmeshBuilder::SetNormals(std::vector<Vector3D>&& normals)
{
	assert(normals.size() == m_positions.size());
	m_normals = std::move(normals);
}

void SubmeshBuilder::SetTangents(std::vector<Vector3D>&& tangents)
{
	assert(tangents.size() == m_positions.size());
	m_tangents = std::move(tangents);
}

void SubmeshBuilder::SetBitangents(std::vector<Vector3D>&& bitangents)
{
	assert(bitangents.size() == m_positions.size());
	m_normals = std::move(bitangents);
}

void SubmeshBuilder::SetColors(std::vector<Vector4D>&& colors)
{
	assert(colors.size() % m_positions.size() == 0);
	m_colors = std::move(colors);
}

void SubmeshBuilder::SetTexcoords(std::vector<float>&& texcoords, const uint8_t channel)
{
	assert(texcoords.size() % m_positions.size() == 0);
	switch (channel)
	{
		case 0:
			m_texcoords0 = std::move(texcoords);
			break;
		case 1:
			m_texcoords1 = std::move(texcoords);
			break;
		case 2:
			m_texcoords2 = std::move(texcoords);
			break;
		case 3:
			m_texcoords3 = std::move(texcoords);
			break;
		default:
			assert(false);
	}
}

void SubmeshBuilder::SetSkeleton(Skeleton&& skeleton)
{
	m_skeleton = std::move(skeleton);
}

template<typename T>
static void AppendVertexData(std::vector<T>& newData, std::vector<uint8_t>& vertexData, uint32_t& headIndex)
{
	if (newData.size() > 0)
	{
		const uint64_t byteSize = sizeof T * newData.size();
		memcpy(&vertexData[headIndex], &newData[0], byteSize);
		headIndex += byteSize;
	}
}

Submesh SubmeshBuilder::Build()
{
	MeshBuffer indexBuffer = {
		std::move(m_indices),
		sizeof(uint32_t)
	};

	const AttributeUsage attributes = GetAttributeUsage();

	const uint32_t stride = attributes.GetAttributeOffsets().m_stride;

	std::vector<uint8_t> vertices(stride * m_positions.size());

	uint32_t headIndex = 0;

	AppendVertexData(m_positions, vertices, headIndex);

	if (m_normals.size() != 0)
	{
		AppendVertexData(m_normals, vertices, headIndex);
	}
	if (m_tangents.size() != 0)
	{
		AppendVertexData(m_tangents, vertices, headIndex);
	}
	if (m_bitangents.size() != 0)
	{
		AppendVertexData(m_bitangents, vertices, headIndex);
	}
	if (m_colors.size() != 0)
	{
		AppendVertexData(m_colors, vertices, headIndex);
	}
	if (m_texcoords0.size() != 0)
	{
		AppendVertexData(m_texcoords0, vertices, headIndex);
	}
	if (m_texcoords1.size() != 0)
	{
		AppendVertexData(m_texcoords1, vertices, headIndex);
	}
	if (m_texcoords2.size() != 0)
	{
		AppendVertexData(m_texcoords2, vertices, headIndex);
	}
	if (m_texcoords3.size() != 0)
	{
		AppendVertexData(m_texcoords3, vertices, headIndex);
	}

	MeshBuffer vertexBuffer = {
		std::move(vertices),
		stride
	};

	Submesh submesh(std::move(m_name), std::move(vertexBuffer), std::move(indexBuffer), attributes,
		CalculateBoundingBox());
	submesh.SetSkeleton(std::move(m_skeleton));

	return submesh;
}

AttributeUsage SubmeshBuilder::GetAttributeUsage() const
{
	return AttributeUsage
	{
		m_normals.size() != 0,
		m_tangents.size() != 0,
		m_bitangents.size() != 0,
		(uint8_t) (m_colors.size() / m_positions.size()),
		(uint8_t) (m_texcoords0.size() / m_positions.size()),
		(uint8_t) (m_texcoords1.size() / m_positions.size()),
		(uint8_t) (m_texcoords2.size() / m_positions.size()),
		(uint8_t) (m_texcoords3.size() / m_positions.size()),
	};
}

BoundingBox SubmeshBuilder::CalculateBoundingBox() const
{
	BoundingBox bb(m_positions[0], m_positions[0]);
	for (uint32_t i = 1; i < m_positions.size(); ++i)
	{
		bb.Append((Point3D) m_positions[i]);
	}
	return bb;
}
