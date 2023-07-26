#include "stdafx.h"
#include "CullingSystem.h"

#include "Assets/Mesh.h"
#include "Components/CameraComponent.h"
#include "Components/ColliderComponent.h"
#include "Components/MeshComponent.h"
#include "Components/RenderCullComponent.h"
#include "Components/TransformComponent.h"
#include "Entities/Camera.h"
#include "Entities/World.h"
#include "Managers/FrameCounter.h"
#include "Quadtree/QuadtreeNode.h"

namespace Rendering
{
	void CullingSystem::OnUpdateStart()
	{
		//TODO: support multiple cameras
		m_cameraFrustum = Camera::Get()->GetComponentAccessor().GetComponent<CameraComponent>()->m_frustum;
		World::Get()->GetQuadtree().CalculateCulling(m_cameraFrustum, m_frameCounter->m_renderFrameCount);
	}

	void CullingSystem::Update(uint64_t entityId, TransformComponent* transform, MeshComponent* mesh, RenderCullComponent* renderCull,
							   ColliderComponent* collider)
	{
		if (m_frameCounter->m_renderFrameCount != transform->m_quadtreePlacement.GetNode()->GetLastSeenFrameId())
		{
			renderCull->m_isCulled = true;
			return;
		}


		switch (transform->m_quadtreePlacement.GetNode()->GetLastVisibilityTestResult())
		{
			case IntersectTestResult::OUTSIDE:
				renderCull->m_isCulled = true;
				break;
			case IntersectTestResult::INTERSECT:
			{
				switch (Intersect(m_cameraFrustum, collider->m_transformedBoundingBox))
				{
					case IntersectTestResult::OUTSIDE:
						renderCull->m_isCulled = true;
						break;
					case IntersectTestResult::INTERSECT:
						renderCull->m_isCulled = false;
						renderCull->m_culledSubmeshes.reset();
						if (renderCull->m_cullSubmeshes && mesh->m_mesh->GetSubmeshCount() > 1)
						{
							const Transform4D worldTransform = transform->GetWorldTransform();

							for (uint32_t i = 0; i < mesh->m_mesh->GetSubmeshCount(); ++i)
							{
								const Submesh& submesh        = mesh->m_mesh->GetSubmesh(i);
								const BoundingBox boundingBox = submesh.GetBoundingBox().Transform(worldTransform);

								if (Intersect(m_cameraFrustum, boundingBox) == IntersectTestResult::OUTSIDE)
								{
									renderCull->m_culledSubmeshes.set(i);
								}
							}
						}
						break;
					case IntersectTestResult::INSIDE:
						renderCull->m_isCulled = false;
						renderCull->m_culledSubmeshes.reset();
						break;
				}
			}
			break;
			case IntersectTestResult::INSIDE:
				renderCull->m_isCulled = false;
				renderCull->m_culledSubmeshes.reset();
				break;
		}
	}
}
