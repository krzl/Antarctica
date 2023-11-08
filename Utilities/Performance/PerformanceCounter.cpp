#include "stdafx.h"
#include "PerformanceCounter.h"

#include "PerformanceMonitor.h"

PerformanceCounter::PerformanceCounter(const uint64_t id, const std::string& name)
{
	timer.Start();
	m_monitor = &PerformanceMonitor::GetMonitor();
	m_monitor->OpenNode(id, name);
}

PerformanceCounter::PerformanceCounter(const std::string& name) :
	PerformanceCounter(std::hash<std::string>{}(name), name) { }

PerformanceCounter::~PerformanceCounter()
{
	m_monitor->CloseNode(timer.GetTime());
	timer.Stop();
}
