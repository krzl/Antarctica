#include "stdafx.h"
#include "MeshObject.h"
#include "SubmeshData.h"

namespace Renderer
{
	void MeshObject::Init(const std::vector<Submesh>& submeshes)
	{
		m_submeshObjects.resize(submeshes.size());

		for (uint32_t i = 0; i < submeshes.size(); ++i)
		{
			m_submeshObjects[i].SetSubmesh(&submeshes[i]);
		}
	}

	void MeshObject::Bind(const uint32_t submeshIndex)
	{
		assert(submeshIndex < m_submeshObjects.size());
		m_submeshObjects[submeshIndex].Bind();
	}
}
