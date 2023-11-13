#pragma once

#include "Performance/PerformanceMonitor.h"

#include "Systems/System.h"

class PerformanceMonitorSystem : public SystemBase
{
	void Run() override;

	void ProcessNode(const PerformanceMonitor::Node& node);

	float m_fpsCounterLastUpdate = 0.0f;
	uint32_t m_fpsCounterFramesElapsed = 0;

	uint32_t m_currentFps = 0.0f;
	
};
