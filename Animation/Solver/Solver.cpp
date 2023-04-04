#include "stdafx.h"
#include "Solver.h"

#include "Animator/Animator.h"

namespace Anim
{
	void Solver::ResetSolver(const std::shared_ptr<Animator>& animator)
	{
		m_animator = animator;
		m_stateMachineData.clear();
		m_stateMachineData.resize(animator->m_stateMachines.size());
		m_triggerState.clear();

		for (uint32_t i = 0; i < animator->m_stateMachines.size(); ++i)
		{
			const std::shared_ptr<StateMachine> stateMachine = animator->m_stateMachines[i];
		}
	}

	static std::vector<std::vector<Matrix4D>> TransposeTransforms(const MeshBoneTransforms& transforms)
	{
		std::vector<std::vector<Matrix4D>> transposed(transforms.size());

		for (uint32_t i = 0; i < transforms.size(); ++i)
		{
			transposed[i].resize(transforms[i].size());
			for (uint32_t j = 0; j < transforms[i].size(); ++j)
			{
				transposed[i][j] = transforms[i][j].transpose;
			}
		}
		return transposed;
	}


	std::vector<std::vector<Matrix4D>> Solver::UpdateAnimation(const std::shared_ptr<Mesh>& mesh)
	{
		std::vector<const Skeleton*> skeletons(mesh->GetSubmeshCount());

		for (uint32_t i = 0; i < mesh->GetSubmeshCount(); ++i)
		{
			skeletons[i] = &mesh->GetSubmesh(i).GetSkeleton();
		}

		for (uint32_t i = 0; i < m_animator->m_stateMachines.size(); ++i)
		{
			std::shared_ptr<StateMachine> stateMachine = m_animator->m_stateMachines[i];

			const MeshBoneTransforms boneTransforms = m_animator->m_stateMachines[i]->CalculateMatrices(
				m_stateMachineData[i], m_triggerState, skeletons);
			//TODO: Support multiple layers

			return TransposeTransforms(boneTransforms);
		}
		return {};
	}

	void CalculateNode(const AnimationNode* node, const std::vector<const Skeleton*>& skeletons, float currentTime,
					   MeshBoneTransforms&  transforms, const Transform4D&            parentTransform)
	{
		Transform4D nodeTransform = Transform4D::identity;// node->m_baseTransform; TODO: remove??

		if (node->m_positionKeys.size() != 0)
		{
			Vector3D   translation;
			Quaternion rotation;
			Vector3D   scale;

			if (node->m_positionKeys.back().m_time > currentTime)
			{
				for (uint32_t i = 1; i < node->m_positionKeys.size(); ++i)
				{
					if (node->m_positionKeys[i].m_time > currentTime)
					{
						const float alpha = InverseLerp(node->m_positionKeys[i - 1].m_time,
														node->m_positionKeys[i].m_time, currentTime);
						translation = LerpClamped(node->m_positionKeys[i - 1].m_position,
												  node->m_positionKeys[i].m_position,
												  alpha);
						break;
					}
				}
			}
			else
			{
				translation = node->m_positionKeys.back().m_position;
			}

			if (node->m_rotationKeys.back().m_time > currentTime)
			{
				for (uint32_t i = 1; i < node->m_rotationKeys.size(); ++i)
				{
					if (node->m_rotationKeys[i].m_time > currentTime)
					{
						const float alpha = InverseLerp(node->m_rotationKeys[i - 1].m_time,
														node->m_rotationKeys[i].m_time, currentTime);
						rotation = SlerpClamped(node->m_rotationKeys[i - 1].m_rotation,
												node->m_rotationKeys[i].m_rotation,
												alpha);
						break;
					}
				}
			}
			else
			{
				rotation = node->m_rotationKeys.back().m_rotation;
			}

			if (node->m_scaleKeys.back().m_time > currentTime)
			{
				for (uint32_t i = 1; i < node->m_scaleKeys.size(); ++i)
				{
					if (node->m_scaleKeys[i].m_time > currentTime)
					{
						const float alpha = InverseLerp(node->m_scaleKeys[i - 1].m_time, node->m_scaleKeys[i].m_time, currentTime);
						scale = LerpClamped(node->m_scaleKeys[i - 1].m_scale, node->m_scaleKeys[i].m_scale, alpha);
						break;
					}
				}
			}
			else
			{
				scale = node->m_scaleKeys.back().m_scale;
			}

			nodeTransform = Transform4D::MakeTranslation(translation) * rotation.GetRotationMatrix() *
							Transform4D::MakeScale(scale.x, scale.y, scale.z);
		}

		Transform4D globalTransformation = parentTransform * nodeTransform;

		for (uint32_t i = 0; i < skeletons.size(); ++i)
		{
			const Skeleton* skeleton = skeletons[i];

			int32_t boneId = skeleton->GetBoneId(node);
			if (boneId != -1)
			{
				transforms[i][boneId] = skeleton->m_globalInverseTransform * globalTransformation *
										skeleton->m_bones[boneId].m_offsetMatrix;
			}
		}

		for (AnimationNode* children : node->m_children)
		{
			CalculateNode(children, skeletons, currentTime, transforms, globalTransformation);
		}
	}

	MeshBoneTransforms Solver::Calculate(const std::shared_ptr<Animation>&   animation,
										 const std::vector<const Skeleton*>& skeletons,
										 const float                         currentTime)
	{
		MeshBoneTransforms transforms(skeletons.size());
		for (uint32_t i = 0; i < skeletons.size(); ++i)
		{
			std::vector<Transform4D>& transform = transforms[i];
			transform.resize(skeletons[0]->m_bones.size());
		}
		const Transform4D initialTransform = Transform4D::identity;
		CalculateNode(animation->m_rootNode, skeletons, currentTime, transforms, initialTransform);
		return transforms;
	}

	MeshBoneTransforms Solver::Interpolate(const MeshBoneTransforms& aTransforms,
										   const MeshBoneTransforms& bTransforms,
										   const float               alpha)
	{
		assert(aTransforms.size() == bTransforms.size());

		MeshBoneTransforms meshTransforms(aTransforms.size());

		for (uint32_t i = 0; i < aTransforms.size(); ++i)
		{
			assert(aTransforms[i].size() == bTransforms[i].size());

			std::vector<Transform4D>& transforms = meshTransforms[i];

			for (uint32_t j = 0; j < aTransforms.size(); ++j)
			{
				Vector3D   aTranslation;
				Quaternion aRotation;
				Vector3D   aScale;

				Vector3D   bTranslation;
				Quaternion bRotation;
				Vector3D   bScale;

				DecomposeTransform(aTransforms[i][j], aTranslation, aRotation, aScale);
				DecomposeTransform(bTransforms[i][j], bTranslation, bRotation, bScale);

				const Vector3D   translation = LerpClamped(aTranslation, bTranslation, alpha);
				const Quaternion rotation    = SlerpClamped(aRotation, bRotation, alpha);
				const Vector3D   scale       = LerpClamped(aScale, bScale, alpha);

				transforms[j] = Transform4D::MakeTranslation(translation) * rotation.GetRotationMatrix() *
								Transform4D::MakeScale(scale.x, scale.y, scale.z);
			}
		}

		return meshTransforms;
	}
}
