#pragma once

#include "System.h"

#include <Time/Timer.h>

class TimeSystem : public System
{

protected:
	
	virtual void Init() override;
	virtual void Update() override;

private:

	Timer m_timer;
};

