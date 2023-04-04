#include "stdafx.h"
#include "StateMachine.h"

#include "State.h"
#include "StateMachineData.h"
#include "Transition.h"
#include "Systems/TimeSystem.h"

namespace Anim
{
	bool Transition::CanTransition(const std::set<int>& triggerState,
								   const bool           isCurrentStateFinished) const
	{
		if (!isCurrentStateFinished && !m_canInterrupt)
		{
			return false;
		}

		for (int trigger : m_transitionConditions)
		{
			auto it = triggerState.find(trigger);
			if (it == triggerState.end())
			{
				return false;
			}
		}

		return true;
	}

	MeshBoneTransforms StateMachine::CalculateMatrices(StateMachineData&                  stateMachineData,
													   std::set<int>&                     triggerState,
													   const std::vector<const Skeleton*> skeletons) const
	{
		if (stateMachineData.m_currentState == nullptr)
		{
			stateMachineData.m_currentState = m_states[0].get();
		}

		const Transition* transition = stateMachineData.m_currentState->GetNextState(
			triggerState, stateMachineData.m_currentTime);

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
			stateMachineData.m_currentTime += TimeSystem::GetInstance()->GetDeltaTime();

			if (stateMachineData.m_previousState != nullptr && stateMachineData.m_currentTime > stateMachineData.
				m_transitionTime)
			{
				stateMachineData.m_previousState = nullptr;
			}
		}

		MeshBoneTransforms transforms = stateMachineData.m_currentState->CalculateBones(
			skeletons, stateMachineData.m_currentTime);

		if (stateMachineData.m_previousState)
		{
			const MeshBoneTransforms oldStateTransforms = stateMachineData.m_previousState->
																		   CalculateBones(skeletons,
																					      stateMachineData.
																					      m_previousTime);

			const float alpha = stateMachineData.m_currentTime / stateMachineData.m_transitionTime;

			transforms = Solver::Interpolate(oldStateTransforms, transforms, alpha);
		}

		return transforms;
	}
}
