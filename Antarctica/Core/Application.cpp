#include "stdafx.h"
#include "Application.h"

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

}

void Application::Render()
{

}
