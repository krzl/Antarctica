#include "stdafx.h"
#include "RenderSystem.h"

#include "RenderObject.h"
#include "Assets/Mesh.h"
#include "Components/AnimatedMeshComponent.h"
#include "Components/CameraComponent.h"
#include "Components/MeshComponent.h"
#include "Components/RenderComponent.h"
#include "Components/RenderCullComponent.h"
#include "Components/TransformComponent.h"
#include "Entities/Camera.h"
#include "Entities/World.h"

namespace Rendering
{
	struct RenderQueueComp
	{
		bool operator()(const QueuedRenderObject* a, const QueuedRenderObject* b) const
		{
			if (a == nullptr)
				return a;
			if (b == nullptr)
				return b;
			return (*a < *b);
		}
	};

	void RenderSystem::OnUpdateStart()
	{
		m_counter = 0;
		//TODO: make it expand when almost full
		m_renderQueue.resize(1024 * 64);

		m_cameraFrustum = Camera::Get()->GetComponentAccessor().GetComponent<CameraComponent>()->m_frustum;
	}

	void RenderSystem::OnUpdateEnd()
	{
		m_renderQueue.resize(m_counter);
		std::sort(m_renderQueue.begin(), m_renderQueue.end(), RenderQueueComp());
	}

	void RenderSystem::Update(uint64_t entityId, TransformComponent* transform, MeshComponent* mesh, RenderComponent* render)
	{
		ComponentAccessor          componentAccessor = World::Get()->GetEntity(entityId)->GetComponentAccessor();
		const RenderCullComponent* renderCull        = componentAccessor.GetComponent<RenderCullComponent>();
		if (renderCull && renderCull->m_isCulled)
		{
			return;
		}

		if (!mesh->m_mesh || mesh->m_materials.empty())
		{
			return;
		}

		Transform4D worldTransform = transform->GetWorldTransform();

		render->m_renderHandles.resize(mesh->m_mesh->GetSubmeshCount());

		for (uint32_t i = 0; i < mesh->m_mesh->GetSubmeshCount(); ++i)
		{
			if (renderCull && renderCull->m_cullSubmeshes && renderCull->m_culledSubmeshes.test(i))
			{
				continue;
			}

			QueuedRenderObject& renderHandle = render->m_renderHandles[i];

			Material* material = &*mesh->m_materials[0];
			if (mesh->m_materials.size() > i && mesh->m_materials[i])
			{
				material = &*mesh->m_materials[i];
			}

			const Transform4D worldMatrix = worldTransform * GetAttachmentTransform(componentAccessor, mesh, i);

			renderHandle.m_submesh                     = &mesh->m_mesh->GetSubmesh(i);
			renderHandle.m_material                    = material;
			renderHandle.m_order                       = material->GetOrder();
			renderHandle.m_perObjectBuffer             = PerObjectBuffer::DEFAULT_BUFFER;
			renderHandle.m_perObjectBuffer.m_transform = worldMatrix.transpose;

			if (i < mesh->m_rectMasks.size() && mesh->m_rectMasks[i].has_value())
			{
				renderHandle.m_clipRect = mesh->m_rectMasks[i].value();
			}
			else
			{
				renderHandle.m_clipRect.reset();
			}

			if (renderHandle.m_boneTransforms.size() != 0)
			{
				for (uint32_t j = 0; j < renderHandle.m_boneTransforms.size(); ++j)
				{
					renderHandle.m_boneTransforms[j] = renderHandle.m_boneTransforms[j] * renderHandle.m_perObjectBuffer.m_transform;
				}

				renderHandle.m_perObjectBuffer.m_transform = Transform4D::identity;
			}

			m_renderQueue[m_counter.fetch_add(1)] = &renderHandle;
		}
	}

	Transform4D RenderSystem::GetAttachmentTransform(ComponentAccessor& componentAccessor, const MeshComponent* mesh, const uint32_t submeshId)
	{
		const Submesh& submesh = mesh->m_mesh->GetSubmesh(submeshId);
		if (submesh.GetAttachmentNodeId() != -1)
		{
			return GetAttachedNodeTransform(componentAccessor, *mesh->m_mesh, submesh.GetAttachmentNodeId(), submesh.GetIgnoreAttachmentRotation());
		}

		return Transform4D::identity;
	}

	Transform4D RenderSystem::GetAttachedNodeTransform(ComponentAccessor& componentAccessor,
		const Mesh&                                                       mesh,
		int32_t                                                           nodeId,
		bool                                                              ignoreAttachmentRotation)
	{
		if (Anim::AnimatedMeshComponent* animatedMesh = componentAccessor.GetComponent<Anim::AnimatedMeshComponent>())
		{
			Transform4D transform = animatedMesh->m_animationSolver.GetNodeTransforms()[nodeId];
			if (ignoreAttachmentRotation)
			{
				Vector3D   translation;
				Quaternion rotation;
				Vector3D   scale;

				DecomposeTransform(transform, translation, rotation, scale);

				transform = Transform4D(Matrix3D::MakeScale(scale.x, scale.y, scale.z), translation);
			}

			return transform;
		}

		return mesh.GetNodes()[nodeId].m_globalTransform;
	}
}
