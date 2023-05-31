#pragma once

namespace Anim
{
	class State;

	struct TriggerCondition
	{
		int32_t m_triggerId;
		bool    m_shouldConsume   = false;
		bool    m_invertCondition = false;
	};

	struct Transition
	{

		bool CanTransition(const std::set<int32_t>& triggerState, bool isCurrentStateFinished) const;

		bool                          m_canInterrupt      = false;
		float                         m_transitionTime    = 0.0f;
		std::vector<TriggerCondition> m_triggerConditions = {};
		State*                        m_state             = nullptr;
	};
}
