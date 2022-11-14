#include "stdafx.h"
#include "StaticMeshComponent.h"

std::vector<Renderer::RenderHandle> StaticMeshComponent::PrepareForRender()
{
	if (!m_mesh)
		return {};

	std::vector<Renderer::RenderHandle> renderHandles;

	auto& submeshObjects = m_mesh->GetMeshObject().GetSubmeshObjects();

	for (uint32_t i = 0; i < m_mesh->GetSubmeshCount(); ++i)
	{
		if (m_materials.size() > i && m_materials[i])
		{
			UpdateConstantBuffer();

			renderHandles.push_back(Renderer::RenderHandle(
														   submeshObjects[i],
														   m_materials[i]->GetMaterialObject(),
														   m_constantBuffer,
														   m_mesh->GetSubmesh(i).GetAttributes(),
														   m_materials[i]->GetOrder())
								   );
		}
	}

	return renderHandles;
}
