#pragma once
#include "Types.h"

namespace Anim
{
	class State;

	struct TriggerCondition
	{
		int32_t m_triggerId;
		bool m_shouldConsume   = false;
		bool m_invertCondition = false;
	};

	struct Transition
	{
		bool CanTransition(const TriggerState& triggerState, bool isCurrentStateFinished) const;

		State* m_state         = nullptr;
		float m_transitionTime = 0.0f;
		bool m_canInterrupt    = false;

		std::vector<TriggerCondition> m_triggerConditions = {};
	};
}
