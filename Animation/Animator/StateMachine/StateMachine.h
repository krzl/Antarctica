#pragma once

#include <iosfwd>
#include <vector>

#include "Types.h"
#include "Assets/Mesh.h"

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

		std::vector<Transform4D> CalculateMatrices(StateMachineData&            stateMachineData,
												   std::set<int>&               triggerState,
												   const std::vector<MeshNode>& meshNodes) const;

	private:

		const std::vector<std::shared_ptr<State>> m_states;
	};
}
