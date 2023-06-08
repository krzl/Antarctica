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
	}

	Transform4D StaticMeshComponent::GetAttachedNodeTransform(const int32_t nodeId, bool ignoreAttachmentRotation)
	{
		return m_mesh->GetNodes()[nodeId].m_globalTransform;
	}

	std::vector<QueuedRenderObject> StaticMeshComponent::PrepareForRender()
	{
		if (!m_mesh)
			return {};

		std::vector<QueuedRenderObject> renderHandles;

		for (uint32_t i = 0; i < m_mesh->GetSubmeshCount(); ++i)
		{
			if (m_materials.size() > i && m_materials[i])
			{
				std::vector<uint8_t> constantBuffer = GetConstantBuffer(i);

				renderHandles.push_back(QueuedRenderObject(
						m_mesh->GetSubmesh(i),
						*m_materials[i],
						std::move(constantBuffer)
					)
				);
			}
		}

		return renderHandles;
	}
}
