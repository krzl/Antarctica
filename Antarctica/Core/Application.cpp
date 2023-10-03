#include "stdafx.h"
#include "Application.h"

#include "Camera/PlayerCameraSystem.h"
#include "Entities/World.h"
#include "Managers/Manager.h"

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

	CreateSystems();

	m_window.SetupInputManager(*InputManager::GetInstance());

	OnApplicationInitialized.Dispatch();

	Run();
}

void Application::Run()
{
	m_isRunning = true;
	TimeManager::GetInstance()->RestartTimer();

	while (m_isRunning)
	{
		if (!m_isPaused)
		{
			m_window.Update();

			++m_frameCounter.m_renderFrameCount;


			for (Manager* manager : m_managers)
			{
				manager->Update();
			}
			m_world.Update();

			m_ecs.RunBeginFrame();

			m_frameCounter.CalculateStepLockFramesToExecute();

			while (m_frameCounter.m_stepLockFramesPending > 0)
			{
				--m_frameCounter.m_stepLockFramesPending;
				++m_frameCounter.m_stepLockFrameCount;
				m_ecs.RunStepLock();
			}
		}

		m_ecs.RunEndFrame();

		m_renderer.Render(GetSystem<Rendering::RenderSystem>()->GetRenderQueue(), GetSystem<PlayerCameraSystem>()->GetCameras());
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
