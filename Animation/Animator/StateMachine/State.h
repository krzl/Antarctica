#pragma once

#include "Transition.h"
#include "Solver/Solver.h"

namespace Anim
{
	class State
	{
		friend class StateMachine;

	public:

		virtual ~State() = default;

		void AddTransition(Transition&& transition, bool isReversible);


		virtual bool IsFinished(float currentTime) const { return true; }

	protected:

		virtual void CalculateBones(std::vector<Transform4D>& boneMatrices,
			const std::vector<MeshNode>&                      meshNodes,
			const float                                       currentTime) const = 0;

	private:

		const Transition* GetNextState(TriggerState& triggerState, float currentTime) const;

		std::vector<Transition> m_transitions;
	};
}
