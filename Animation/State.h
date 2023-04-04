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

		virtual bool IsFinished(float currentTime) const { return true; }

	protected:

		virtual MeshBoneTransforms CalculateBones(std::vector<const Skeleton*> skeletons,
												  float                        currentTime) const = 0;

	private:

		const Transition* GetNextState(std::set<int>& triggerState, float currentTime) const;

		std::vector<Transition> m_transitions;
	};
}
