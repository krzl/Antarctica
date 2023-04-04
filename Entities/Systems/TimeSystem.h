#pragma once

#include <Time/Timer.h>
#include "Systems/System.h"

class TimeSystem : public System
{
	REGISTER_SYSTEM(TimeSystem);

public:

	[[nodiscard]] float GetDeltaTime() const { return m_deltaTime; }
	[[nodiscard]] float GetTimeSinceStart() const { return m_timeSinceStart; }

protected:

	void Init() override;
	void Update() override;

private:

	Timer m_timer;

	float m_deltaTime      = 0.0f;
	float m_timeSinceStart = 0.0f;
};
