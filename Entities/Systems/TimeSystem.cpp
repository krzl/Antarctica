#include "stdafx.h"
#include "TimeSystem.h"

void TimeSystem::Init()
{
	m_timer.Start();
}

void TimeSystem::Update()
{
	const float oldTime = m_timeSinceStart;
	m_timeSinceStart    = m_timer.GetTime();
	m_deltaTime         = m_timeSinceStart - oldTime;
}
