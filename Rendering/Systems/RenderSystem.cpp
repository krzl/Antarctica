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
		m_renderQueue.resize(1024 * 64 * 64);

		m_cameraFrustum = Camera::Get()->GetComponentAccessor().GetComponent<CameraComponent>()->m_frustum;
	}

	void RenderSystem::OnUpdateEnd()
	{
		m_renderQueue.resize(m_counter);
		std::sort(m_renderQueue.begin(), m_renderQueue.end(), RenderQueueComp());
	}

	void RenderSystem::Update(Entity* entity, TransformComponent* transform, MeshComponent* mesh, RenderComponent* render)
	{
		if (!render->m_isEnabled)
		{
			return;
		}

		const ComponentAccessor& componentAccessor = entity->GetComponentAccessor();
		const RenderCullComponent* renderCull      = componentAccessor.GetComponent<RenderCullComponent>();

		uint32_t handlesCount = 0;
		for (uint32_t i = 0; i < mesh->m_renderItems.size(); ++i)
		{
			const RenderItem& renderItem = mesh->m_renderItems[i];

			if (renderCull && renderCull->m_cullData[i].m_isCulled)
			{
				continue;
			}

			if (!renderItem.m_mesh || renderItem.m_materials.empty() || renderItem.m_isHidden)
			{
				continue;
			}

			handlesCount += renderItem.m_mesh->GetSubmeshCount();
		}

		render->m_renderHandles.resize(handlesCount);

		uint32_t currentHandleId = 0;
		
		for (uint32_t i = 0; i < mesh->m_renderItems.size(); ++i)
		{
			const RenderItem& renderItem = mesh->m_renderItems[i];

			if (renderCull && renderCull->m_cullData[i].m_isCulled)
			{
				continue;
			}
			
			if (!renderItem.m_mesh || renderItem.m_materials.empty() || renderItem.m_isHidden)
			{
				continue;
			}

			Transform4D worldTransform = render->m_worldTransform;

			for (uint32_t j = 0; j < renderItem.m_mesh->GetSubmeshCount(); ++j)
			{
				if (renderCull && renderCull->m_cullSubmeshes && renderCull->m_cullData[i].m_culledSubmeshes.test(j))
				{
					continue;
				}

				QueuedRenderObject& renderHandle = render->m_renderHandles[currentHandleId++];

				Material* material = &*renderItem.m_materials[0];
				if (renderItem.m_materials.size() > j && renderItem.m_materials[j])
				{
					material = &*renderItem.m_materials[j];
				}

				const Transform4D worldMatrix = worldTransform * renderItem.m_transform * GetAttachmentTransform(componentAccessor, renderItem, j);

				renderHandle.m_submesh                        = &renderItem.m_mesh->GetSubmesh(j);
				renderHandle.m_material                       = material;
				renderHandle.m_order                          = material->GetOrder();
				renderHandle.m_perObjectBuffer                = PerObjectBuffer::DEFAULT_BUFFER;
				renderHandle.m_perObjectBuffer.m_transform    = worldMatrix.transpose;
				renderHandle.m_perObjectBuffer.m_transformInv = Inverse(worldMatrix).transpose;

				if (j < renderItem.m_rectMasks.size() && renderItem.m_rectMasks[j].has_value())
				{
					renderHandle.m_clipRect = renderItem.m_rectMasks[j].value();
				}
				else
				{
					renderHandle.m_clipRect.reset();
				}

				if (renderHandle.m_boneTransforms.size() != 0)
				{
					for (uint32_t k = 0; k < renderHandle.m_boneTransforms.size(); ++k)
					{
						renderHandle.m_boneTransforms[k] = renderHandle.m_boneTransforms[k] * renderHandle.m_perObjectBuffer.m_transform;
					}

					renderHandle.m_perObjectBuffer.m_transform = Transform4D::identity;
				}

				m_renderQueue[m_counter.fetch_add(1)] = &renderHandle;
			}
		}
	}

	Transform4D RenderSystem::GetAttachmentTransform(const ComponentAccessor& componentAccessor, const RenderItem& renderItem, const uint32_t submeshId)
	{
		const Submesh& submesh = renderItem.m_mesh->GetSubmesh(submeshId);
		if (submesh.GetAttachmentNodeId() != -1)
		{
			return GetAttachedNodeTransform(componentAccessor, renderItem, submesh.GetAttachmentNodeId(), submesh.GetIgnoreAttachmentRotation());
		}

		return Transform4D::identity;
	}

	Transform4D RenderSystem::GetAttachedNodeTransform(const ComponentAccessor& componentAccessor, const RenderItem& renderItem, int32_t nodeId,
													   bool ignoreAttachmentRotation)
	{
		if (renderItem.m_isAnimated)
		{
			if (Anim::AnimatedMeshComponent* animatedMesh = componentAccessor.GetComponent<Anim::AnimatedMeshComponent>())
			{
				Transform4D transform = animatedMesh->m_animationSolver.GetNodeTransforms()[nodeId];
				if (ignoreAttachmentRotation)
				{
					Vector3D translation;
					Quaternion rotation;
					Vector3D scale;

					DecomposeTransform(transform, translation, rotation, scale);

					transform = Transform4D(Matrix3D::MakeScale(scale.x, scale.y, scale.z), translation);
				}

				return transform;
			}
		}

		return renderItem.m_mesh->GetNodes()[nodeId].m_globalTransform;
	}
}
