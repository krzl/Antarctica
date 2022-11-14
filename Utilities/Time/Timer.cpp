#include "stdafx.h"
#include "Timer.h"

using namespace std::chrono_literals;

void Timer::Start()
{
	if (!m_isStopped)
	{
		return;
	}
	
	m_isStopped = false;

	m_startTime = std::chrono::steady_clock::now();
}

void Timer::Stop()
{
	if (m_isStopped)
	{
		return;
	}

	m_isStopped = true;
	m_accumulator += GetTime();
}

void Timer::Restart()
{
	m_accumulator = 0.0f;
	m_isStopped = true;
	Start();
}

float Timer::GetTime() const
{
	return m_accumulator + (std::chrono::steady_clock::now() - m_startTime) / 1us / 10000000.0f;
}
