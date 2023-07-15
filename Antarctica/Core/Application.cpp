#include "stdafx.h"
#include "Application.h"

#include <RenderSystem.h>

#include "Entities/CameraComponent.h"
#include "GameObjects/World.h"
#include "Systems/System.h"
#include "Systems/TimeSystem.h"

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
		m_renderSystem.OnResize(m_window);
	});

	m_window.OnDestroyed.AddListener([this]()
	{
		m_isRunning = false;
	});

	m_renderSystem.Init(m_window, m_appSettings);

	m_systems = SystemCreator::CreateSystems();
	for (System* system : m_systems)
	{
		system->Init();
	}

	m_window.SetupInputSystem(*InputSystem::GetInstance());

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
			for (System* system : m_systems)
			{
				system->Update();
			}
			Update(TimeSystem::GetInstance()->GetDeltaTime());
		}
		m_window.Update();
		Renderer::CameraComponent::SetAspectRatio(GetWindow().GetAspectRatio());
		std::vector<GameObject*> gameObjectsToRender = GetWorld().GetQuadtree().Intersect(
			Renderer::CameraComponent::Get()->GetFrustum());
		m_renderSystem.Render(gameObjectsToRender);
	}

	m_renderSystem.Cleanup();
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

void Application::Update(const float deltaTime)
{
	m_world.Update(deltaTime);
}

Application& Application::Get()
{
	return *gApp;
}
