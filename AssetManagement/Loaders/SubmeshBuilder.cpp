#include "stdafx.h"
#include "SubmeshBuilder.h"

SubmeshBuilder::SubmeshBuilder(std::vector<Vector3D>&& positions, std::vector<uint32_t>& indices) :
	m_indices(reinterpret_cast<uint8_t*>(indices.data()), reinterpret_cast<uint8_t*>(indices.data()) + indices.size() * 4),
	m_positions(std::move(positions))
{
}

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

Renderer::Submesh SubmeshBuilder::Build()
{
	Renderer::MeshBuffer indexBuffer = {
		std::move(m_indices),
		sizeof(uint32_t)
	};

	const Renderer::AttributeUsage attributes = GetAttributeUsage();

	const uint32_t stride = GetStride();

	std::vector<uint8_t> vertices(stride * m_positions.size());

	uint32_t headIndex = 0;

	for (uint32_t i = 0; i < m_positions.size(); i++)
	{
		memcpy(&vertices[headIndex], &m_positions[i], sizeof m_positions[i]);
		headIndex += sizeof m_positions[i];

		if (m_normals.size() != 0)
		{
			memcpy(&vertices[headIndex], &m_normals[i], sizeof m_normals[i]);
			headIndex += sizeof m_normals[i];
		}
		if (m_tangents.size() != 0)
		{
			memcpy(&vertices[headIndex], &m_tangents[i], sizeof m_tangents[i]);
			headIndex += sizeof m_tangents[i];
		}
		if (m_bitangents.size() != 0)
		{
			memcpy(&vertices[headIndex], &m_bitangents[i], sizeof m_bitangents[i]);
			headIndex += sizeof m_bitangents[i];
		}
		if (m_colors.size() != 0)
		{
			memcpy(&vertices[headIndex], &m_colors[i], sizeof m_colors[i] * attributes.m_colorChannelCount);
			headIndex += sizeof m_colors[i] * attributes.m_colorChannelCount;
		}
		if (m_texcoords0.size() != 0)
		{
			memcpy(&vertices[headIndex], &m_texcoords0[i], sizeof m_texcoords0[i] * attributes.m_dataSizeTexcoord0);
			headIndex += sizeof m_texcoords0[i] * attributes.m_dataSizeTexcoord0;
		}
		if (m_texcoords1.size() != 0)
		{
			memcpy(&vertices[headIndex], &m_texcoords1[i], sizeof m_texcoords1[i] * attributes.m_dataSizeTexcoord1);
			headIndex += sizeof m_texcoords1[i] * attributes.m_dataSizeTexcoord1;
		}
		if (m_texcoords2.size() != 0)
		{
			memcpy(&vertices[headIndex], &m_texcoords2[i], sizeof m_texcoords2[i] * attributes.m_dataSizeTexcoord2);
			headIndex += sizeof m_texcoords2[i] * attributes.m_dataSizeTexcoord2;
		}
		if (m_texcoords3.size() != 0)
		{
			memcpy(&vertices[headIndex], &m_texcoords3[i], sizeof m_texcoords3[i] * attributes.m_dataSizeTexcoord3);
			headIndex += sizeof m_texcoords3[i] * attributes.m_dataSizeTexcoord3;
		}
	}

	Renderer::MeshBuffer vertexBuffer = {
		std::move(vertices),
		stride
	};

	Renderer::Submesh submesh(std::move(vertexBuffer), std::move(indexBuffer), attributes);

	return submesh;
}

uint32_t SubmeshBuilder::GetStride() const
{
	uint32_t stride = 3 * 4;

	if (m_normals.size() != 0)
	{
		stride += 3 * 4;
	}
	if (m_tangents.size() != 0)
	{
		stride += 3 * 4;
	}
	if (m_bitangents.size() != 0)
	{
		stride += 3 * 4;
	}
	if (m_colors.size() != 0)
	{
		stride += 4 * (uint32_t) m_colors.size() / (uint32_t) m_positions.size();
	}
	if (m_texcoords0.size() != 0)
	{
		stride += 4 * (uint32_t) m_texcoords0.size() / (uint32_t) m_positions.size();
	}
	if (m_texcoords1.size() != 0)
	{
		stride += 4 * (uint32_t) m_texcoords1.size() / (uint32_t) m_positions.size();
	}
	if (m_texcoords2.size() != 0)
	{
		stride += 4 * (uint32_t) m_texcoords2.size() / (uint32_t) m_positions.size();
	}
	if (m_texcoords3.size() != 0)
	{
		stride += 4 * (uint32_t) m_texcoords3.size() / (uint32_t) m_positions.size();
	}

	return stride;
}

Renderer::AttributeUsage SubmeshBuilder::GetAttributeUsage() const
{
	return Renderer::AttributeUsage
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
