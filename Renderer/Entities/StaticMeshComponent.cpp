#include "stdafx.h"
#include "StaticMeshComponent.h"

#include "Buffers/Types/PerObjectBuffer.h"

namespace Renderer
{

	Transform4D StaticMeshComponent::GetAttachmentTransform(const uint32_t submeshId)
	{
		const Submesh& submesh = m_mesh->GetSubmesh(submeshId);
		if (submesh.GetAttachmentNodeId() != -1)
		{
			return GetAttachedNodeTransform(submesh.GetAttachmentNodeId(), submesh.GetIgnoreAttachmentRotation());
		}
		else
		{	
			return RenderComponent::GetAttachmentTransform(submeshId);
		}
	}

	void StaticMeshComponent::SetMesh(const std::shared_ptr<Mesh>& mesh)
	{
		m_mesh = mesh;

		m_constantBuffers.clear();
		m_constantBuffers.resize(mesh->GetSubmeshCount());
		for (uint32_t i = 0; i < m_constantBuffers.size(); ++i)
		{
			m_constantBuffers[i].Init(1, sizeof(PerObjectBuffer), &PerObjectBuffer::DEFAULT_BUFFER);
		}
	}

	Transform4D StaticMeshComponent::GetAttachedNodeTransform(const int32_t nodeId, bool ignoreAttachmentRotation)
	{
		return m_mesh->GetNodes()[nodeId].m_globalTransform;
	}

	std::vector<RenderHandle> StaticMeshComponent::PrepareForRender()
	{
		if (!m_mesh)
			return {};

		std::vector<RenderHandle> renderHandles;

		for (uint32_t i = 0; i < m_mesh->GetSubmeshCount(); ++i)
		{
			if (m_materials.size() > i && m_materials[i])
			{
				UpdateConstantBuffer(i);

				renderHandles.push_back(RenderHandle(
						m_mesh->GetSubmesh(i),
						*m_materials[i],
						m_constantBuffers[i])
				);
			}
		}

		return renderHandles;
	}
}
