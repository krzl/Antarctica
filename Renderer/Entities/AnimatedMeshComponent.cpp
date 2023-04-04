#include "stdafx.h"
#include "AnimatedMeshComponent.h"

namespace Renderer
{
	std::unordered_set<AnimatedMeshComponent*> animatedMeshComponents;

	void AnimatedMeshComponent::OnEnabled()
	{
		StaticMeshComponent::OnEnabled();
		animatedMeshComponents.insert(this);
	}

	void AnimatedMeshComponent::OnDisabled()
	{
		StaticMeshComponent::OnDisabled();
		animatedMeshComponents.erase(this);
	}

	std::vector<SkinningData> AnimatedMeshComponent::GetAllSkinningData()
	{
		std::vector<SkinningData> skinningData;

		for (AnimatedMeshComponent* component : animatedMeshComponents)
		{
			if (component->m_mesh && component->m_animator)
			{
				std::vector<std::vector<Matrix4D>> boneTransforms = component->m_animationSolver.UpdateAnimation(
					component->m_mesh);
				uint32_t offset = 0;

				for (uint32_t i = 0; i < component->m_mesh->GetSubmeshCount(); ++i)
				{
					const uint32_t bonesCount = static_cast<uint32_t>(boneTransforms[i].size());

					skinningData.emplace_back(SkinningData{
						boneTransforms[i],
						component->m_weightsBuffer,
						component->m_transformBuffer,
						component->m_skinningOutputBuffers[i],
						component->m_mesh->GetSubmesh(i),
						offset
					});
					offset += bonesCount;
				}
			}
		}

		return skinningData;
	}

	std::vector<RenderHandle> AnimatedMeshComponent::PrepareForRender()
	{
		std::vector<RenderHandle> handles = StaticMeshComponent::PrepareForRender();
		
		if (m_skinningOutputBuffers.size() != m_mesh->GetSubmeshCount())
		{
			m_skinningOutputBuffers.resize(m_mesh->GetSubmeshCount());
		}

		for (uint32_t i = 0; i < handles.size(); ++i)
		{
			RenderHandle& handle = handles[i];
			handle.m_skinningBuffers = &m_skinningOutputBuffers[i];
		}

		return handles;
	}

	void AnimatedMeshComponent::SetAnimator(const std::shared_ptr<Anim::Animator> animator)
	{
		m_animator = animator;
		m_animationSolver.ResetSolver(m_animator);
	}
}
