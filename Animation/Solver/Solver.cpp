#include "stdafx.h"
#include "Solver.h"

#include "Animator/Animator.h"

namespace Anim
{
	void Solver::SetTrigger(const int32_t id, const bool value)
	{
		if (value)
		{
			m_triggerState.emplace(id);
		}
		else
		{
			m_triggerState.erase(id);
		}
	}

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

	static int32_t FindNodeId(const std::vector<MeshNode>& nodes, const std::string& name)
	{
		for (uint32_t i = 0; i < nodes.size(); ++i)
		{
			if (nodes[i].m_name == name)
			{
				return i;
			}
		}
		return -1;
	}

	void CalculateNode(const AnimationNode*      node, const std::vector<MeshNode>& meshNodes, float currentTime,
					   std::vector<Transform4D>& transforms, const Transform4D&     parentTransform)
	{
		int32_t meshNodeId = FindNodeId(meshNodes, node->m_nodeName);

		Transform4D nodeTransform = node->m_baseTransform;
		if (meshNodeId != -1)
		{
			nodeTransform = meshNodes[meshNodeId].m_localTransform;
		}

		if (node->m_positionKeys.size() != 0 || node->m_rotationKeys.size() != 0 || node->m_scaleKeys.size() != 0)
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
						const float alpha = InverseLerp(node->m_scaleKeys[i - 1].m_time, node->m_scaleKeys[i].m_time,
														currentTime);
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

			Vector3D x = rotation.GetDirectionX();
		}

		Transform4D globalTransformation = parentTransform * nodeTransform;


		if (meshNodeId != -1)
		{
			transforms[meshNodeId] = globalTransformation;
		}

		for (AnimationNode* children : node->m_children)
		{
			CalculateNode(children, meshNodes, currentTime, transforms, globalTransformation);
		}
	}

	std::vector<Transform4D> Solver::Calculate(const std::shared_ptr<Animation>& animation,
											   const std::vector<MeshNode>&      meshNodes,
											   const float                       currentTime)
	{
		std::vector<Transform4D> transforms(meshNodes.size());
		const Transform4D        initialTransform = Transform4D::identity;
		CalculateNode(animation->m_rootNode, meshNodes, currentTime, transforms, initialTransform);

		for (uint32_t i = 0; i < meshNodes.size(); ++i)
		{
			if (transforms[i].m33 == 0.0f && meshNodes[i].m_parentNodeId >= 0)
			{
				transforms[i] = transforms[meshNodes[i].m_parentNodeId] * meshNodes[i].m_localTransform;
			}
		}
		return transforms;
	}

	std::vector<std::vector<Matrix4D>> Solver::UpdateAnimation(const std::shared_ptr<Mesh>& mesh)
	{
		std::vector<std::vector<Matrix4D>> finalMatrices(mesh->GetSubmeshCount());

		for (uint32_t i = 0; i < m_animator->m_stateMachines.size(); ++i)
		{
			std::shared_ptr<StateMachine> stateMachine = m_animator->m_stateMachines[i];

			//TODO: Support multiple layers
			m_nodeTransforms = m_animator->m_stateMachines[i]->CalculateMatrices(
				m_stateMachineData[i], m_triggerState, mesh->GetNodes());

			break;
		}

		for (uint32_t i = 0; i < mesh->GetSubmeshCount(); ++i)
		{
			const Skeleton& skeleton = mesh->GetSubmesh(i).GetSkeleton();

			finalMatrices[i].resize(skeleton.m_bones.size());

			for (uint32_t j = 0; j < skeleton.m_bones.size(); ++j)
			{
				Matrix4D      finalMatrix;
				const int32_t id = FindNodeId(mesh->GetNodes(), skeleton.m_bones[j].m_boneName);
				if (id != -1)
				{
					finalMatrix = skeleton.m_globalInverseTransform * m_nodeTransforms[id] * skeleton.m_bones[j].
								  m_offsetMatrix;
				}
				else
				{
					finalMatrix = Transform4D::identity;
				}
				finalMatrices[i][j] = finalMatrix.transpose;
			}
		}

		return finalMatrices;
	}

	std::vector<Transform4D> Solver::Interpolate(const std::vector<Transform4D>& aTransforms,
												 const std::vector<Transform4D>& bTransforms,
												 const float                     alpha)
	{
		assert(aTransforms.size() == bTransforms.size());

		std::vector<Transform4D> interpolated;
		interpolated.resize(aTransforms.size());

		for (uint32_t i = 0; i < aTransforms.size(); ++i)
		{
			Vector3D   aTranslation;
			Quaternion aRotation;
			Vector3D   aScale;

			Vector3D   bTranslation;
			Quaternion bRotation;
			Vector3D   bScale;

			DecomposeTransform(aTransforms[i], aTranslation, aRotation, aScale);
			DecomposeTransform(bTransforms[i], bTranslation, bRotation, bScale);

			const Vector3D   translation = LerpClamped(aTranslation, bTranslation, alpha);
			const Quaternion rotation    = SlerpClamped(aRotation, bRotation, alpha);
			const Vector3D   scale       = LerpClamped(aScale, bScale, alpha);

			interpolated[i] = Transform4D::MakeTranslation(translation) * rotation.GetRotationMatrix() *
							  Transform4D::MakeScale(scale.x, scale.y, scale.z);
		}

		return interpolated;
	}
}
