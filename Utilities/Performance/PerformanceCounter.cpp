#include "stdafx.h"
#include "PerformanceCounter.h"

#include "PerformanceMonitor.h"

PerformanceCounter::PerformanceCounter(const uint64_t id, const std::string& name)
{
	m_monitor = &PerformanceMonitor::GetMonitor();
	m_monitor->OpenNode(id, name);
	timer.Start();
}

PerformanceCounter::PerformanceCounter(const std::string& name) :
	PerformanceCounter(std::hash<std::string>{}(name), name) { }

PerformanceCounter::~PerformanceCounter()
{
	timer.Stop();
	m_monitor->CloseNode(timer.GetTime());
}
