#pragma once
#include "TimeManager.h"

class FrameCounter
{
public:

	uint32_t m_renderFrameCount;
	uint32_t m_stepLockFrameCount;
	uint32_t m_stepLockFramesPending;

	void CalculateStepLockFramesToExecute()
	{
		m_stepLockFramesPending = Terathon::Floor(GetNextStepLockProgress());
	}

	float GetNextStepLockProgress() const
	{
		const TimeManager* timeManager  = TimeManager::GetInstance();
		const float stepLockTimeElapsed = m_stepLockFrameCount * timeManager->GetTimeStep();
		const float timeNotInStepLock   = timeManager->GetTimeSinceStart() - stepLockTimeElapsed;
		return timeNotInStepLock / timeManager->GetTimeStep();
	}
};
