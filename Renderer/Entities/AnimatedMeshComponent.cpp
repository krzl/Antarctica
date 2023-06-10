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

	void AnimatedMeshComponent::SetupRenderHandle(const uint32_t submeshId, QueuedRenderObject& renderObject) 
	{
		StaticMeshComponent::SetupRenderHandle(submeshId, renderObject);
		
		const Submesh&      submesh    = m_mesh->GetSubmesh(submeshId);
		const uint32_t      bonesCount = static_cast<uint32_t>(m_animationSolver.GetFinalMatrices()[submeshId].size());

		if (submesh.GetSkeleton().m_bones.size() == 0 || bonesCount == 0)
		{
			return;
		}

		renderObject.m_boneTransforms.resize(bonesCount);

		for (uint32_t j = 0; j < bonesCount; ++j)
		{
			renderObject.m_boneTransforms[j] = m_animationSolver.GetFinalMatrices()[submeshId][j] * renderObject.m_perObjectBuffer.m_transform;
		}

		renderObject.m_perObjectBuffer.m_transform = Transform4D::identity;
	}

	void AnimatedMeshComponent::PrepareForRender(RenderQueue& renderQueue)
	{
		if (m_mesh && m_animator)
		{
			m_animationSolver.UpdateAnimation(m_mesh);
		}

		StaticMeshComponent::PrepareForRender(renderQueue);
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
