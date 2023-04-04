#include "stdafx.h"
#include "StateMachineBuilder.h"

namespace Anim
{
	std::shared_ptr<StateMachine> StateMachineBuilder::Build(std::shared_ptr<State> initialState)
	{
#if _DEBUG
		if (m_states.size() == 0)
		{
			throw "No states were added to state machine node";
		}
#endif
		if (initialState == nullptr)
		{
			initialState = m_states[0];
		}
#if _DEBUG
		else
		{
			const auto it = std::find(m_states.begin(), m_states.end(), initialState);
			if (it == m_states.end())
			{
				throw "Invalid state set as initial state for state machine node";
			}
		}
#endif
		return std::make_shared<StateMachine>(std::move(m_states));
	}
}
