#pragma once

#include "Animator/../Types.h"
#include "StateMachineData.h"
#include "Assets/Mesh.h"

namespace Anim
{
	struct StateMachineData;
	class StateMachine;
	class Node;
	class Animator;

	class Solver
	{
	public:

		void ResetSolver(const std::shared_ptr<Animator>& animator);

		bool IsAnimatorCurrent(const std::shared_ptr<Animator>& animator) const { return animator == m_animator; }

		static Anim::MeshBoneTransforms Calculate(const std::shared_ptr<Animation>&   animation,
												  const std::vector<const Skeleton*>& skeletons,
												  float                               currentTime);
		static Anim::MeshBoneTransforms Interpolate(const MeshBoneTransforms& aTransforms,
													const MeshBoneTransforms& bTransforms, float alpha);

		std::vector<std::vector<Matrix4D>> UpdateAnimation(const std::shared_ptr<Mesh>& mesh);

	private:

		std::shared_ptr<Animator>     m_animator         = nullptr;
		std::vector<StateMachineData> m_stateMachineData = {};
		std::set<int>                 m_triggerState     = {};
	};
}
