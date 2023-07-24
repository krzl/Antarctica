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

		void Update(StateMachineData& stateMachineData, TriggerState& triggerState) const;
		void CalculateMatrices(StateMachineData& stateMachineData, std::vector<Transform4D>& matrices, const std::vector<MeshNode>& meshNodes) const;

	private:

		const std::vector<std::shared_ptr<State>> m_states;
	};
}
