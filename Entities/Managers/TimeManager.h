#pragma once

#include <Time/Timer.h>
#include "Managers/Manager.h"

class TimeManager : public Manager
{
	REGISTER_MANAGER(TimeManager);

public:

	[[nodiscard]] float GetDeltaTime() const { return min(1.0f, m_deltaTime); }
	[[nodiscard]] float GetTimeSinceStart() const { return m_timeSinceStart; }

protected:

	void Init() override;
	void Update() override;

private:

	Timer m_timer;

	float m_deltaTime      = 0.0f;
	float m_timeSinceStart = 0.0f;
};
