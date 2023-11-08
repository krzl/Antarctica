#include "stdafx.h"
#include "ECS.h"

#include "Performance/PerformanceMonitor.h"

#include "Systems/SystemBase.h"

void ECS::ExecuteForAllSystems(const std::function<void(SystemBase*)> function) const
{
	for (SystemBase* system : m_preStepLockSystems)
	{
		function(system);
	}
	for (SystemBase* system : m_stepLockSystems)
	{
		function(system);
	}
	for (SystemBase* system : m_postStepLockSystems)
	{
		function(system);
	}
}

SystemBase* ECS::GetSystem(const uint64_t hash)
{
	const auto it = m_lookupMap.find(hash);
	if (it != m_lookupMap.end())
	{
		return it->second;
	}
	return nullptr;
}

void ECS::RunBeginFrame()
{
	PERF_COUNTER(BeginFrame);
	ExecuteForAllSystems([](SystemBase* system)
	{
		system->OnFrameStart();
	});

	for (SystemBase* system : m_preStepLockSystems)
	{
		PerformanceCounter counter(typeid(*system).name());
		system->Run();
	}
}

void ECS::RunStepLock()
{
	PERF_COUNTER(LockStep);
	ExecuteForAllSystems([](SystemBase* system)
	{
		system->OnStepLockStart();
	});

	for (SystemBase* system : m_stepLockSystems)
	{
		PerformanceCounter counter(typeid(*system).name());
		system->Run();
	}

	ExecuteForAllSystems([](SystemBase* system)
	{
		system->OnStepLockEnd();
	});
}

void ECS::RunEndFrame()
{
	PERF_COUNTER(EndFrame);
	for (SystemBase* system : m_postStepLockSystems)
	{
		PerformanceCounter counter(typeid(*system).name());
		system->Run();
	}

	ExecuteForAllSystems([](SystemBase* system)
	{
		system->OnFrameEnd();
	});
}

ECS::~ECS()
{
	ExecuteForAllSystems([](const SystemBase* system)
	{
		delete system;
	});
}
