#pragma once

#include "Types.h"

struct Skeleton;

namespace Anim
{
	struct StateMachineData;
	class State;

	class StateMachine
	{
		friend class StateMachineBuilder;

	public:

		StateMachine() = default;

		explicit StateMachine(std::vector<std::shared_ptr<State>>&& states) :
			m_states(std::move(states)) { }

		MeshBoneTransforms CalculateMatrices(StateMachineData&            stateMachineData,
											 std::set<int>&               triggerState,
											 std::vector<const Skeleton*> skeletons) const;

	private:

		const std::vector<std::shared_ptr<State>> m_states;
	};
}
