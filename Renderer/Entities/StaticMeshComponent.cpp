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

	void StaticMeshComponent::SetupRenderHandle(const uint32_t submeshId, QueuedRenderObject& renderObject)
	{
		renderObject.m_submesh        = &m_mesh->GetSubmesh(submeshId);
		renderObject.m_material       = &*m_materials[submeshId];
		renderObject.m_order          = renderObject.m_material->GetOrder();
		renderObject.m_perObjectBuffer = GetConstantBuffer(submeshId);
		renderObject.m_boneTransforms.clear();
	}


	void StaticMeshComponent::PrepareForRender(RenderQueue& renderQueue)
	{
		if (!m_mesh)
			return;

		for (uint32_t i = 0; i < m_mesh->GetSubmeshCount(); ++i)
		{
			if (m_materials.size() > i && m_materials[i])
			{

				static uint32_t id = 0;

				SetupRenderHandle(i, renderQueue.emplace_back());

				id = (id + 1) % 4097;
			}
		}
	}
}
