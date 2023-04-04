#pragma once

namespace Anim
{
	class State;

	struct Transition
	{
		bool CanTransition(const std::set<int>& triggerState, bool isCurrentStateFinished) const;

		bool             m_canInterrupt         = false;
		float            m_transitionTime       = 0.0f;
		std::vector<int> m_transitionConditions = {};
		State*           m_state                = nullptr;
	};
}
