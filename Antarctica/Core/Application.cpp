#include "stdafx.h"
#include "Application.h"

#include <Renderer.h>

#include "Abilities/AbilitySystem.h"
#include "Abilities/AbilityTriggerSystem.h"
#include "Camera/PlayerCameraSystem.h"
#include "Debug/DebugDraw/DebugDrawSystem.h"
#include "Debug/ImGui/ImGuiSystem.h"
#include "Entities/World.h"
#include "Input/InputSystem.h"
#include "Managers/Manager.h"
#include "Steering/MovementSystem.h"
#include "Steering/SteeringSystem.h"
#include "Systems/AnimationSystem.h"
#include "Systems/CullingSystem.h"
#include "Systems/QuadtreeUpdateSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/SkinningSystem.h"

Application* gApp = nullptr;

void SetDefaultAppSettings(Settings& appSettings)
{
	appSettings.SetValue("window.title", "Antarctica");
	appSettings.SetValue("window.width", 1920);
	appSettings.SetValue("window.height", 1080);
}

void Application::Start()
{
	if (m_isRunning)
		return;

	gApp = this;

	SetDefaultAppSettings(m_appSettings);
	m_appSettings.LoadSettings("../Config/AppSettings.ini");
#if _DEBUG
	m_appSettings.SaveSettings();
#endif
	m_window.Init(m_appSettings);

	m_window.OnResized.AddListener([this]()
	{
		m_renderer.OnResize(m_window);
	});

	m_window.OnDestroyed.AddListener([this]()
	{
		m_isRunning = false;
	});

	m_renderer.Init(m_window, m_appSettings);

	m_managers = ManagerCreator::CreateManagers();
	for (Manager* manager : m_managers)
	{
		manager->Init();
	}

	m_inputSystem        = new InputSystem();
	m_playerCameraSystem = new PlayerCameraSystem();

	m_preStepLockSystems = std::initializer_list<SystemBase*>{
		m_inputSystem,
		m_playerCameraSystem,
		new AbilityTriggerSystem(),
	};

	m_stepLockSystems = std::initializer_list<SystemBase*>{
		new AbilitySystem(),
		new Anim::AnimationSystem(),
		new Navigation::SteeringSystem(),
		new Navigation::MovementSystem(),
		new QuadtreeUpdateSystem(),
	};

	m_renderSystem = new Rendering::RenderSystem();

	m_postStepLockSystems = std::initializer_list<SystemBase*>{
		new DebugDrawSystem(),
		new Rendering::CullingSystem(),
		new Rendering::SkinningSystem(),
		new ImGuiSystem(),
		m_renderSystem
	};

	for (SystemBase* system : m_preStepLockSystems)
	{
		system->Init(&m_frameCounter);
	}
	for (SystemBase* system : m_stepLockSystems)
	{
		system->Init(&m_frameCounter);
	}
	for (SystemBase* system : m_postStepLockSystems)
	{
		system->Init(&m_frameCounter);
	}

	m_window.SetupInputManager(*InputManager::GetInstance());

	OnApplicationInitialized.Dispatch();

	Run();
}

void Application::Run()
{
	m_isRunning = true;

	while (m_isRunning)
	{
		if (!m_isPaused)
		{
			m_inputSystem->ResetInput();
			m_window.Update();

			++m_frameCounter.m_renderFrameCount;

			for (SystemBase* system : m_preStepLockSystems)
			{
				system->OnFrameBegin();
			}
			for (SystemBase* system : m_stepLockSystems)
			{
				system->OnFrameBegin();
			}
			for (SystemBase* system : m_postStepLockSystems)
			{
				system->OnFrameBegin();
			}

			for (Manager* manager : m_managers)
			{
				manager->Update();
			}
			m_world.Update();

			for (SystemBase* system : m_preStepLockSystems)
			{
				system->Run();
			}

			++m_frameCounter.m_lockStepFrameCount;
			for (SystemBase* system : m_stepLockSystems)
			{
				system->Run();
			}
		}

		for (SystemBase* system : m_postStepLockSystems)
		{
			system->Run();
		}

		for (SystemBase* system : m_preStepLockSystems)
		{
			system->OnFrameEnd();
		}
		for (SystemBase* system : m_stepLockSystems)
		{
			system->OnFrameEnd();
		}
		for (SystemBase* system : m_postStepLockSystems)
		{
			system->OnFrameEnd();
		}

		m_renderer.Render(m_renderSystem->GetRenderQueue(), m_playerCameraSystem->GetCameras());
	}

	m_renderer.Cleanup();
}

void Application::Stop()
{
	m_isRunning = false;
}

void Application::Pause()
{
	m_isPaused = true;
}

void Application::Unpause()
{
	m_isPaused = false;
}

Application& Application::Get()
{
	return *gApp;
}
