#include "stdafx.h"
#include "TimeManager.h"

void TimeManager::Init()
{
	m_timer.Start();
}

void TimeManager::Update()
{
	const float oldTime = m_timeSinceStart;
	m_timeSinceStart    = m_timer.GetTime();
	m_deltaTime         = m_timeSinceStart - oldTime;
}

void TimeManager::RestartTimer()
{
	m_timer.Restart();
}
