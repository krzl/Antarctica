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
				if (component->m_skinningOutputBuffers.size() != component->m_mesh->GetSubmeshCount())
				{
					component->m_skinningOutputBuffers.resize(component->m_mesh->GetSubmeshCount());
				}
				
				std::vector<std::vector<Matrix4D>> boneTransforms = component->m_animationSolver.UpdateAnimation(
					component->m_mesh);
				uint32_t offset = 0;

				for (uint32_t i = 0; i < component->m_mesh->GetSubmeshCount(); ++i)
				{
					const uint32_t bonesCount = static_cast<uint32_t>(boneTransforms[i].size());
					if (component->m_mesh->GetSubmesh(i).GetSkeleton().m_bones.size() != 0)
					{
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
		}

		return skinningData;
	}

	Transform4D AnimatedMeshComponent::GetAttachedNodeTransform(const int32_t nodeId, bool ignoreAttachmentRotation)
	{
		Transform4D transform = m_animationSolver.GetNodeTransforms()[nodeId];
		if (ignoreAttachmentRotation)
		{
			Vector3D   translation;
			Quaternion rotation;
			Vector3D   scale;
			
			DecomposeTransform(transform, translation, rotation, scale);

			transform = Transform4D::MakeTranslation(translation) * Transform4D::MakeScale(scale.x, scale.y, scale.z);
		}
		
		return transform;
	}

	std::vector<RenderHandle> AnimatedMeshComponent::PrepareForRender()
	{
		std::vector<RenderHandle> handles = StaticMeshComponent::PrepareForRender();

		static uint32_t m = 0;
		for (uint32_t i = 0; i < handles.size(); ++i)
		{
			RenderHandle& handle     = handles[i];
			handle.m_skinningBuffer = &m_skinningOutputBuffers[i];
		}
		m++;

		return handles;
	}

	void AnimatedMeshComponent::SetMesh(const std::shared_ptr<Mesh>& mesh)
	{
		StaticMeshComponent::SetMesh(mesh);

		const std::vector<MeshNode>& meshNodes = mesh->GetNodes();
		
		m_animatedTransforms.resize(meshNodes.size());

		for (uint32_t i = 0; i < meshNodes.size(); ++i)
		{
			m_animatedTransforms[i] = meshNodes[i].m_globalTransform;
		}
	}

	void AnimatedMeshComponent::SetAnimator(const std::shared_ptr<Anim::Animator> animator)
	{
		m_animator = animator;
		m_animationSolver.ResetSolver(m_animator);
	}

	void AnimatedMeshComponent::SetTrigger(const int32_t id, const bool value)
	{
		m_animationSolver.SetTrigger(id, value);
	}
}
