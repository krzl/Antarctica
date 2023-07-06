#pragma once
#include "SubmeshData.h"

struct DynamicSubmesh : Submesh
{
	DynamicSubmesh() : Submesh()
	{
		m_isDynamic = true;
	}

	[[nodiscard]] MeshBuffer& GetVertexBuffer()
	{
		return m_vertexBuffer;
	}

	[[nodiscard]] MeshBuffer& GetIndexBuffer()
	{
		return m_indexBuffer;
	}

	void SetAttributeUsage(const AttributeUsage& usage)
	{
		m_attributes = usage;
	}
};
