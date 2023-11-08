#pragma once

#include "Performance/PerformanceMonitor.h"

#include "Systems/System.h"

class PerformanceMonitorSystem : public SystemBase
{
	void Run() override;

	void ProcessNode(const PerformanceMonitor::Node& node);
};
