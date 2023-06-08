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

	std::vector<QueuedRenderObject> AnimatedMeshComponent::PrepareForRender()
	{
		std::vector<std::vector<Matrix4D>> boneTransforms;
		if (m_mesh && m_animator)
		{
			boneTransforms = m_animationSolver.UpdateAnimation(m_mesh);
		}

		std::vector<QueuedRenderObject> handles = StaticMeshComponent::PrepareForRender();

		if (handles.size() == 0 || !m_mesh || !m_animator)
		{
			return handles;
		}

		uint32_t handleId = 0;

		for (uint32_t i = 0; i < m_mesh->GetSubmeshCount(); ++i)
		{
			const Submesh& submesh    = m_mesh->GetSubmesh(i);
			const uint32_t bonesCount = static_cast<uint32_t>(boneTransforms[i].size());

			if (submesh.GetSkeleton().m_bones.size() == 0)
			{
				continue;
			}
			
			handles[i].m_skinningBuffer = &m_skinningBuffers[i];

			if (bonesCount != 0)
			{
				handles[i].m_boneTransforms = std::move(boneTransforms[i]);
			}

			if (handles[i].m_submesh == &submesh)
			{
				handleId++;
			}
		}

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

		m_skinningBuffers.resize(m_mesh->GetSubmeshCount());
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
