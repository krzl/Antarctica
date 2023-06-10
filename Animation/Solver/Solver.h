#pragma once

#include "Animator/../Types.h"
#include "Animator/StateMachine/StateMachineData.h"
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

		void SetTrigger(int32_t id, bool value);

		void ResetSolver(const std::shared_ptr<Animator>& animator);

		bool IsAnimatorCurrent(const std::shared_ptr<Animator>& animator) const { return animator == m_animator; }

		static std::vector<Transform4D> Calculate(const std::shared_ptr<Animation>& animation,
												  const std::vector<MeshNode>&      meshNodes,
												  float                             currentTime);
		static std::vector<Transform4D> Interpolate(const std::vector<Transform4D>& aTransforms,
													const std::vector<Transform4D>& bTransforms, float alpha);

		std::vector<std::vector<Matrix4D>>&                     UpdateAnimation(const std::shared_ptr<Mesh>& mesh);
		[[nodiscard]] const std::vector<std::vector<Matrix4D>>& GetFinalMatrices() const { return m_finalMatrices; }
		[[nodiscard]] const std::vector<Transform4D>&           GetNodeTransforms() const { return m_nodeTransforms; }

	private:

		std::shared_ptr<Animator>     m_animator         = nullptr;
		std::vector<StateMachineData> m_stateMachineData = {};
		std::set<int32_t>             m_triggerState     = {};
		std::vector<Transform4D>      m_nodeTransforms;

		std::vector<std::vector<Matrix4D>> m_finalMatrices;
	};
}
