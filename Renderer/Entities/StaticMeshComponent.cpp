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

		return RenderComponent::GetAttachmentTransform(submeshId);
	}

	void StaticMeshComponent::SetMesh(const std::shared_ptr<Mesh>& mesh)
	{
		m_mesh = mesh;

		m_renderHandles.clear();
		m_renderHandles.resize(m_mesh->GetSubmeshCount());
	}

	Transform4D StaticMeshComponent::GetAttachedNodeTransform(const int32_t nodeId, bool ignoreAttachmentRotation)
	{
		return m_mesh->GetNodes()[nodeId].m_globalTransform;
	}

	void StaticMeshComponent::SetupRenderHandle(const uint32_t      submeshId, Material& material,
												QueuedRenderObject& renderObject)
	{
		renderObject.m_submesh         = &m_mesh->GetSubmesh(submeshId);
		renderObject.m_material        = &material;
		renderObject.m_order           = material.GetOrder();
		renderObject.m_perObjectBuffer = GetConstantBuffer(submeshId);
	}

	void StaticMeshComponent::PrepareForRender(RenderQueue& renderQueue)
	{
		if (!m_mesh || m_materials.empty())
		{
			return;
		}

		for (uint32_t i = 0; i < m_mesh->GetSubmeshCount(); ++i)
		{
			Material* material = &*m_materials[0];
			if (m_materials.size() > i && m_materials[i])
			{
				material = &*m_materials[i];
			}
			SetupRenderHandle(i, *material, m_renderHandles[i]);
		}

		std::lock_guard lock(renderQueueMutex);

		for (uint32_t i = 0; i < m_mesh->GetSubmeshCount(); ++i)
		{
			renderQueue.emplace_back(&m_renderHandles[i]);
		}
	}
}
