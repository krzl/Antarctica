#include "stdafx.h"
#include "StaticMeshComponent.h"

namespace Renderer
{
	std::vector<RenderHandle> StaticMeshComponent::PrepareForRender()
	{
		if (!m_mesh)
			return {};

		std::vector<RenderHandle> renderHandles;

		for (uint32_t i = 0; i < m_mesh->GetSubmeshCount(); ++i)
		{
			if (m_materials.size() > i && m_materials[i])
			{
				UpdateConstantBuffer();

				renderHandles.push_back(RenderHandle(
						m_mesh->GetSubmesh(i),
						*m_materials[i],
						m_constantBuffer)
				);
			}
		}

		return renderHandles;
	}
}
