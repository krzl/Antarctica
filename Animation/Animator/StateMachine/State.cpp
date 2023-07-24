#include "stdafx.h"
#include "State.h"

#include "Transition.h"

namespace Anim
{
	void State::AddTransition(Transition&& transition, const bool isReversible)
	{
		if (isReversible)
		{
			std::vector<TriggerCondition> invertedConditions;

			for (const TriggerCondition& triggerCondition : transition.m_triggerConditions)
			{
				if (!triggerCondition.m_shouldConsume)
				{
					invertedConditions.push_back(TriggerCondition
						{
							triggerCondition.m_triggerId,
							false,
							!triggerCondition.m_invertCondition
						}
					);
				}
			}

			transition.m_state->AddTransition(
				Transition
				{
					transition.m_canInterrupt,
					transition.m_transitionTime,
					invertedConditions,
					this
				},
				false
			);
		}

		m_transitions.emplace_back(std::move(transition));
	}

	const Transition* State::GetNextState(TriggerState& triggerState,
		const float                                                    currentTime) const
	{
		const bool isCurrentStateFinished = IsFinished(currentTime);

		for (const Transition& transition : m_transitions)
		{
			if (transition.CanTransition(triggerState, isCurrentStateFinished))
			{
				for (const TriggerCondition& triggerCondition : transition.m_triggerConditions)
				{
					if (triggerCondition.m_shouldConsume && !triggerCondition.m_invertCondition)
					{
						triggerState.set(triggerCondition.m_triggerId, false);
					}
				}
				return &transition;
			}
		}

		return nullptr;
	}
}
