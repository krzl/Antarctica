#include "stdafx.h"
#include "Mesh.h"

void Mesh::AddSubmesh(Renderer::Submesh&& submesh)
{
	m_submeshes.push_back(std::move(submesh));
}
