#include "stdafx.h"
#include "SkinningSystem.h"

#include "RenderObject.h"
#include "Components/AnimatedMeshComponent.h"
#include "Components/MeshComponent.h"
#include "Components/RenderComponent.h"
#include "Components/RenderCullComponent.h"
#include "Entities/World.h"

namespace Rendering
{
	void SkinningSystem::Update(Entity* entity, Anim::AnimatedMeshComponent* animatedMesh, MeshComponent* mesh, RenderComponent* render)
	{
		const RenderCullComponent* renderCull = entity->GetComponentAccessor().GetComponent<RenderCullComponent>();

		for (uint32_t i = 0; i < mesh->m_renderItems.size(); ++i)
		{
			RenderItem& renderItem = mesh->m_renderItems[i];
			
			if (renderCull && renderCull->m_cullData[i].m_isCulled)
			{
				continue;
			}

			if (!renderItem.m_isAnimated || renderItem.m_isHidden)
			{
				continue;
			}

			animatedMesh->m_animationSolver.CalculateAnimation(renderItem.m_mesh);

			render->m_renderHandles.resize(renderItem.m_mesh->GetSubmeshCount());

			for (uint32_t i = 0; i < render->m_renderHandles.size(); ++i)
			{
				if (renderCull && renderCull->m_cullSubmeshes && renderCull->m_cullData[i].m_culledSubmeshes.test(i))
				{
					continue;
				}

				QueuedRenderObject& renderObject = render->m_renderHandles[i];

				const std::vector<Matrix4D>& finalMatrices = animatedMesh->m_animationSolver.GetFinalMatrices()[i];

				const Submesh& submesh = renderItem.m_mesh->GetSubmesh(i);
				const uint32_t bonesCount = static_cast<uint32_t>(finalMatrices.size());

				if (submesh.GetSkeleton().m_bones.size() == 0 || bonesCount == 0)
				{
					continue;
				}

				renderObject.m_boneTransforms = finalMatrices;
			}
		}
	}
}
