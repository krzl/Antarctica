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

	static int32_t FindNodeId(const std::vector<MeshNode>& nodes, const uint64_t nameHash)
	{
		for (uint32_t i = 0; i < nodes.size(); ++i)
		{
			if (nodes[i].m_nameHash == nameHash)
			{
				return i;
			}
		}
		return -1;
	}

	template<typename T>
	uint32_t FindKeyId(const std::vector<T>& keys, const float currentTime)
	{
		uint32_t left  = 0;
		uint32_t right = keys.size() - 2;

		while (left <= right)
		{
			const uint32_t mid = left + (right - left) / 2;

			bool a = keys[mid].m_time <= currentTime;
			bool b = keys[mid + 1].m_time >= currentTime;

			if (keys[mid].m_time <= currentTime && keys[mid + 1].m_time >= currentTime)
				return mid;

			if (keys[mid].m_time < currentTime)
				left = mid + 1;
			else
				right = mid - 1;
		}

		return -1;
	}

	static Vector3D GetCurrentTranslation(const AnimationNode* node, const float currentTime)
	{
		if (node->m_positionKeys.size() > 1 && node->m_positionKeys.back().m_time > currentTime)
		{
			const uint32_t keyId = FindKeyId(node->m_positionKeys, currentTime);
			const float    alpha = InverseLerp(node->m_positionKeys[keyId].m_time,
											   node->m_positionKeys[keyId + 1].m_time, currentTime);
			return LerpClamped(node->m_positionKeys[keyId].m_position,
							   node->m_positionKeys[keyId + 1].m_position,
							   alpha);
		}
		return node->m_positionKeys.back().m_position;
	}

	Quaternion GetCurrentRotation(const AnimationNode* node, const float currentTime)
	{
		if (node->m_rotationKeys.size() > 1 && node->m_rotationKeys.back().m_time > currentTime)
		{
			const uint32_t keyId = FindKeyId(node->m_rotationKeys, currentTime);
			const float    alpha = InverseLerp(node->m_rotationKeys[keyId].m_time,
											   node->m_rotationKeys[keyId + 1].m_time, currentTime);
			return SlerpClamped(node->m_rotationKeys[keyId].m_rotation,
								node->m_rotationKeys[keyId + 1].m_rotation,
								alpha);
		}
		return node->m_rotationKeys.back().m_rotation;
	}

	Vector3D GetCurrentScale(const AnimationNode* node, const float currentTime)
	{
		if (node->m_scaleKeys.size() > 1 && node->m_scaleKeys.back().m_time > currentTime)
		{
			const uint32_t keyId = FindKeyId(node->m_scaleKeys, currentTime);
			const float    alpha = InverseLerp(node->m_scaleKeys[keyId].m_time, node->m_scaleKeys[keyId + 1].m_time,
											   currentTime);
			return LerpClamped(node->m_scaleKeys[keyId].m_scale,
							   node->m_scaleKeys[keyId + 1].m_scale,
							   alpha);
		}
		return node->m_scaleKeys.back().m_scale;
	}

	void CalculateNode(const AnimationNode*      node, const std::vector<MeshNode>& meshNodes, float currentTime,
					   std::vector<Transform4D>& transforms, const Transform4D&     parentTransform)
	{
		int32_t meshNodeId = FindNodeId(meshNodes, node->m_nodeNameHash);

		Transform4D nodeTransform = node->m_baseTransform;
		if (meshNodeId != -1)
		{
			nodeTransform = meshNodes[meshNodeId].m_localTransform;
		}

		if (node->m_positionKeys.size() != 0 || node->m_rotationKeys.size() != 0 || node->m_scaleKeys.size() != 0)
		{
			nodeTransform = Transform4D::identity;

			if (node->m_positionKeys.size() != 0)
			{
				nodeTransform = Transform4D::MakeTranslation(GetCurrentTranslation(node, currentTime));
			}

			if (node->m_rotationKeys.size() != 0)
			{
				nodeTransform = nodeTransform * GetCurrentRotation(node, currentTime).GetRotationMatrix();
			}

			if (node->m_scaleKeys.size() != 0)
			{
				const Vector3D scale = GetCurrentScale(node, currentTime);
				nodeTransform        = nodeTransform * Transform4D::MakeScale(scale.x, scale.y, scale.z);
			}
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

	void Solver::Calculate(std::vector<Transform4D>&    transforms, const std::shared_ptr<Animation>& animation,
						   const std::vector<MeshNode>& meshNodes, const float                        currentTime)
	{
		transforms.assign(meshNodes.size(), Transform4D());
		
		const Transform4D initialTransform = Transform4D::identity;
		CalculateNode(animation->m_rootNode, meshNodes, currentTime, transforms, initialTransform);

		for (uint32_t i = 0; i < meshNodes.size(); ++i)
		{
			if (transforms[i].m33 == 0.0f && meshNodes[i].m_parentNodeId >= 0)
			{
				transforms[i] = transforms[meshNodes[i].m_parentNodeId] * meshNodes[i].m_localTransform;
			}
		}
	}

	std::vector<std::vector<Matrix4D>>& Solver::UpdateAnimation(const std::shared_ptr<Mesh>& mesh)
	{
		m_finalMatrices.resize(mesh->GetSubmeshCount());

		for (uint32_t i = 0; i < m_animator->m_stateMachines.size(); ++i)
		{
			std::shared_ptr<StateMachine> stateMachine = m_animator->m_stateMachines[i];

			//TODO: Support multiple layers
			m_animator->m_stateMachines[i]->CalculateMatrices(m_stateMachineData[i], m_nodeTransforms,
															  m_triggerState, mesh->GetNodes());

			break;
		}

		for (uint32_t i = 0; i < mesh->GetSubmeshCount(); ++i)
		{
			const Skeleton& skeleton = mesh->GetSubmesh(i).GetSkeleton();

			m_finalMatrices[i].resize(skeleton.m_bones.size());

			for (uint32_t j = 0; j < skeleton.m_bones.size(); ++j)
			{
				Matrix4D      finalMatrix;
				const int32_t id = FindNodeId(mesh->GetNodes(), skeleton.m_bones[j].m_boneNameHash);
				if (id != -1)
				{
					finalMatrix = skeleton.m_globalInverseTransform * m_nodeTransforms[id] * skeleton.m_bones[j].
								  m_offsetMatrix;
				}
				else
				{
					finalMatrix = Transform4D::identity;
				}
				m_finalMatrices[i][j] = finalMatrix.transpose;
			}
		}

		return m_finalMatrices;
	}

	void Solver::Interpolate(const std::vector<Transform4D>& aTransforms, std::vector<Transform4D>& bTransforms,
							 const float                     alpha)
	{
		assert(aTransforms.size() == bTransforms.size());

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

			bTransforms[i] = Transform4D::MakeTranslation(translation) * rotation.GetRotationMatrix() *
							 Transform4D::MakeScale(scale.x, scale.y, scale.z);
		}
	}
}
