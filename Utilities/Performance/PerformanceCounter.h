#pragma once

#include "Time/Timer.h"

class PerformanceMonitor;

class PerformanceCounter
{
public:

	PerformanceCounter(uint64_t id, const std::string& name);
	explicit PerformanceCounter(const std::string& name);
	~PerformanceCounter();

private:

	Timer timer;
	PerformanceMonitor* m_monitor;
};

#define PERF_COUNTER(name) \
	static const std::string perfCounter##name = #name;\
	static uint64_t perfCounter##name##Hash = std::hash<std::string>{}(perfCounter##name);\
	PerformanceCounter counter(perfCounter##name##Hash, perfCounter##name);
