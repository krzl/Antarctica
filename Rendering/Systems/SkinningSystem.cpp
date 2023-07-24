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
	void SkinningSystem::Update(const uint64_t entityId,
		Anim::AnimatedMeshComponent*           animatedMesh,
		MeshComponent*                         mesh,
		RenderComponent*                       render)
	{
		const RenderCullComponent* renderCull = World::Get()->GetEntity(entityId)->GetComponentAccessor().GetComponent<RenderCullComponent>();
		if (renderCull && renderCull->m_isCulled)
		{
			return;
		}

		animatedMesh->m_animationSolver.CalculateAnimation(mesh->m_mesh);

		render->m_renderHandles.resize(mesh->m_mesh->GetSubmeshCount());

		for (uint32_t i = 0; i < render->m_renderHandles.size(); ++i)
		{
			if (renderCull && renderCull->m_cullSubmeshes && renderCull->m_culledSubmeshes.test(i))
			{
				continue;
			}
			
			QueuedRenderObject& renderObject = render->m_renderHandles[i];

			const std::vector<Matrix4D>& finalMatrices = animatedMesh->m_animationSolver.GetFinalMatrices()[i];

			const Submesh& submesh    = mesh->m_mesh->GetSubmesh(i);
			const uint32_t bonesCount = static_cast<uint32_t>(finalMatrices.size());

			if (submesh.GetSkeleton().m_bones.size() == 0 || bonesCount == 0)
			{
				continue;
			}

			renderObject.m_boneTransforms = finalMatrices;
		}
	}
}
