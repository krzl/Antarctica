#pragma once

namespace Anim
{
	class State;

	struct StateMachineData
	{
		const State* m_currentState  = nullptr;
		const State* m_previousState = nullptr;
		float m_currentTime          = 0.0f;
		float m_previousTime         = 0.0f;
		float m_transitionTime       = 0.0f;
	};
}
