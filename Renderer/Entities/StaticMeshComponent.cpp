#include "stdafx.h"
#include "StaticMeshComponent.h"

#include "Buffers/Types/PerObjectBuffer.h"
#include "GameObjects/GameObject.h"

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

		MarkDirty();
		if (m_owner.IsValid())
		{
			m_owner->MarkDirty();
		}
	}

	BoundingBox StaticMeshComponent::GetBoundingBox() const
	{
		BoundingBox boundingBox = SceneComponent::GetBoundingBox();

		if (m_mesh)
		{
			bool isFirst = true;
			for (const Submesh& submesh : m_mesh->GetSubmeshes())
			{
				BoundingBox submeshBoundingBox = submesh.GetBoundingBox();
				if (isFirst)
				{
					boundingBox = submeshBoundingBox;
					isFirst     = false;
				}
				else
				{
					boundingBox.Append(submeshBoundingBox);
				}
			}
		}

		return boundingBox.Transform(GetWorldTransform());
	}

	float StaticMeshComponent::TraceRay(const RayIntersectionTester& ray, float& closestDistance) const
	{
		if (m_mesh)
		{
			for (const Submesh& submesh : m_mesh->GetSubmeshes())
			{
				BoundingBox submeshBoundingBox = submesh.GetBoundingBox().Transform(GetWorldTransform());
				const float distance           = ray.Intersect(submeshBoundingBox);

				if (distance >= 0.0f && distance < closestDistance)
				{
					if (m_useMeshForCollision)
					{
						//TODO: complex mesh trace
					}
					else
					{
						closestDistance = distance;
					}
				}
			}
		}

		return closestDistance;
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

	void StaticMeshComponent::PrepareForRender(RenderQueue&          renderQueue, const Frustum& cameraFrustum,
											   std::atomic_uint32_t& counter)
	{
		if (!m_mesh || m_materials.empty())
		{
			return;
		}

		for (uint32_t i = 0; i < m_mesh->GetSubmeshCount(); ++i)
		{
			const BoundingBox boundingBox = m_mesh->GetSubmesh(i).GetBoundingBox().Transform(GetWorldTransform());

			if (Intersect(cameraFrustum, boundingBox) == IntersectTestResult::OUTSIDE)
			{
				continue;
			}

			Material* material = &*m_materials[0];
			if (m_materials.size() > i && m_materials[i])
			{
				material = &*m_materials[i];
			}

			SetupRenderHandle(i, *material, m_renderHandles[i]);

			renderQueue[counter.fetch_add(1)] = &m_renderHandles[i];
		}
	}
}
