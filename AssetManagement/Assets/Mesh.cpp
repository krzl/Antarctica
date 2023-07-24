#include "stdafx.h"
#include "Mesh.h"

#include "SubmeshData.h"

void Mesh::AddSubmesh(Submesh&& submesh)
{
	m_submeshes.push_back(std::move(submesh));

	if (m_submeshes.size() == 1)
	{
		m_boundingBox = submesh.GetBoundingBox();
	}
	else
	{
		m_boundingBox.Append(submesh.GetBoundingBox());
	}
}
