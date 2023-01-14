#pragma once

#include "System.h"

#include <Time/Timer.h>

class TimeSystem : public System
{
protected:

	void Init() override;
	void Update() override;

private:

	Timer m_timer;
};
