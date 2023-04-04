#include "stdafx.h"
#include "State.h"

#include "Transition.h"

namespace Anim
{
	const Transition* State::GetNextState(std::set<int>& triggerState,
										  const float    currentTime) const
	{
		const bool isCurrentStateFinished = IsFinished(currentTime);

		for (const Transition& transition : m_transitions)
		{
			if (transition.CanTransition(triggerState, isCurrentStateFinished))
			{
				for (int trigger : transition.m_transitionConditions)
				{
					triggerState.erase(trigger);
				}
				return &transition;
			}
		}

		return nullptr;
	}
}
