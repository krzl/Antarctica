#include "stdafx.h"
#include "Application.h"

#include <RenderSystem.h>

#include "Entities/CameraComponent.h"
#include "Entities/RenderComponent.h"
#include "GameObjects/World.h"

Application* gApp = nullptr;

void SetDefaultAppSettings(Settings& appSettings)
{
	appSettings.SetValue("window.title", "Antarctica");
	appSettings.SetValue("window.width", 1024);
	appSettings.SetValue("window.height", 800);
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
	m_window.Init(m_inputSystem, m_appSettings);

	m_window.OnWindowDestroyed.AddListener([this]()
	{
		m_isRunning = false;
	}, false);

	m_renderSystem.Init(m_window, m_appSettings);

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
			Update();
		}
		m_window.Update();
		Render();
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

void Application::Update()
{
	m_world.Update();
}

void Application::Render()
{
	auto renderQueue = Renderer::RenderComponent::GetRenderQueue();
	Renderer::CameraComponent::SetAspectRatio(GetWindow().GetAspectRatio());
	auto cameraData = Renderer::CameraComponent::GetAllCameraData();
	m_renderSystem.Render(renderQueue, cameraData);
}

Application& Application::Get()
{
	return *gApp;
}
