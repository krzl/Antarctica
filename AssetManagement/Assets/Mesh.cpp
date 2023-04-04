#include "stdafx.h"
#include "Mesh.h"

#include "SubmeshData.h"

void Mesh::AddSubmesh(Submesh&& submesh)
{
	m_submeshes.push_back(std::move(submesh));
}