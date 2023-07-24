#include "stdafx.h"
#include "StateMachine.h"

#include "State.h"
#include "StateMachineData.h"
#include "Transition.h"
#include "Managers/TimeManager.h"

namespace Anim
{
	bool Transition::CanTransition(const TriggerState& triggerState,
		const bool                                     isCurrentStateFinished) const
	{
		if (!isCurrentStateFinished && !m_canInterrupt)
		{
			return false;
		}

		for (const TriggerCondition& triggerCondition : m_triggerConditions)
		{
			const bool isTriggerSet = triggerState.test(triggerCondition.m_triggerId);
			if (isTriggerSet == triggerCondition.m_invertCondition)
			{
				return false;
			}
		}

		return true;
	}

	void StateMachine::Update(StateMachineData& stateMachineData, TriggerState& triggerState) const
	{
		if (stateMachineData.m_currentState == nullptr)
		{
			stateMachineData.m_currentState = m_states[0].get();
		}

		const Transition* transition = stateMachineData.m_currentState->GetNextState(
			triggerState,
			stateMachineData.m_currentTime);

		if (transition != nullptr)
		{
			stateMachineData.m_previousState = transition->m_transitionTime > 0.0f ?
												   stateMachineData.m_currentState :
												   nullptr;
			stateMachineData.m_previousTime = stateMachineData.m_currentTime;

			stateMachineData.m_currentState = transition->m_state;
			stateMachineData.m_currentTime  = 0.0f;

			stateMachineData.m_transitionTime = transition->m_transitionTime;
		}
		else
		{
			stateMachineData.m_currentTime += TimeManager::GetInstance()->GetDeltaTime();

			if (stateMachineData.m_previousState != nullptr && stateMachineData.m_currentTime > stateMachineData.
				m_transitionTime)
			{
				stateMachineData.m_previousState = nullptr;
			}
		}
	}

	void StateMachine::CalculateMatrices(StateMachineData& stateMachineData,
		std::vector<Transform4D>&                          matrices,
		const std::vector<MeshNode>&                       meshNodes) const
	{
		stateMachineData.m_currentState->CalculateBones(matrices,
														meshNodes,
														stateMachineData.m_currentTime);

		if (stateMachineData.m_previousState)
		{
			//TODO: don't reallocate?
			std::vector<Transform4D> oldStateTransforms(matrices.size());

			stateMachineData.m_previousState->CalculateBones(oldStateTransforms,
															 meshNodes,
															 stateMachineData.m_previousTime);

			const float alpha = stateMachineData.m_currentTime / stateMachineData.m_transitionTime;

			Solver::Interpolate(oldStateTransforms, matrices, alpha);
		}
	}
}
