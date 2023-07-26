#include "stdafx.h"
#include "SystemContainer.h"

#include "Abilities/AbilitySystem.h"
#include "Abilities/AbilityTriggerSystem.h"
#include "Camera/PlayerCameraSystem.h"
#include "Debug/DebugDraw/DebugDrawSystem.h"
#include "Debug/ImGui/ImGuiSystem.h"
#include "Input/InputSystem.h"
#include "Steering/MovementSystem.h"
#include "Steering/SteeringSystem.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CullingSystem.h"
#include "Systems/QuadtreeUpdateSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/SkinningSystem.h"


void SystemContainer::CreateSystems()
{
	AddSystem<InputSystem>(PRE_STEP_LOCK);
	AddSystem<PlayerCameraSystem>(PRE_STEP_LOCK);
	AddSystem<AbilityTriggerSystem>(PRE_STEP_LOCK);

	AddSystem<AbilitySystem>(STEP_LOCK);
	AddSystem<Anim::AnimationSystem>(STEP_LOCK);
	AddSystem<Navigation::SteeringSystem>(STEP_LOCK);
	AddSystem<Navigation::MovementSystem>(STEP_LOCK);
	AddSystem<QuadtreeUpdateSystem>(STEP_LOCK);

	AddSystem<DebugDrawSystem>(POST_STEP_LOCK);
	AddSystem<Rendering::CullingSystem>(POST_STEP_LOCK);
	AddSystem<Rendering::SkinningSystem>(POST_STEP_LOCK);
	AddSystem<ImGuiSystem>(POST_STEP_LOCK);
	AddSystem<Rendering::RenderSystem>(POST_STEP_LOCK);

	ExecuteForAllSystems([](SystemBase* system)
	{
		system->Init();
	});
}

void SystemContainer::ExecuteForAllSystems(const std::function<void(SystemBase*)> function) const
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

SystemBase* SystemContainer::GetSystem(const uint64_t hash)
{
	const auto it = m_lookupMap.find(hash);
	if (it != m_lookupMap.end())
	{
		return it->second;
	}
	return nullptr;
}

void SystemContainer::RunBeginFrame()
{
	ExecuteForAllSystems([](SystemBase* system)
	{
		system->OnFrameBegin();
	});

	for (SystemBase* system : m_preStepLockSystems)
	{
		system->Run();
	}
}

void SystemContainer::RunStepLock()
{
	ExecuteForAllSystems([](SystemBase* system)
	{
		system->OnStepLockBegin();
	});

	for (SystemBase* system : m_stepLockSystems)
	{
		system->Run();
	}

	ExecuteForAllSystems([](SystemBase* system)
	{
		system->OnStepLockEnd();
	});
}

void SystemContainer::RunEndFrame()
{
	for (SystemBase* system : m_postStepLockSystems)
	{
		system->Run();
	}

	ExecuteForAllSystems([](SystemBase* system)
	{
		system->OnFrameEnd();
	});
}


SystemContainer::~SystemContainer()
{
	ExecuteForAllSystems([](const SystemBase* system)
	{
		delete system;
	});
}
