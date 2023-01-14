#pragma once

#include <RenderSystem.h>
#include <Window.h>
#include <Input/InputSystem.h>
#include <Settings/Settings.h>

#include "GameObjects/World.h"

class World;

class Application
{
public:

	void Start();
	void Stop();

	void Pause();
	void Unpause();

	void Update();
	void Render();

	static Application& Get();

	[[nodiscard]] InputSystem& GetInputSystem()
	{
		return m_inputSystem;
	}

	[[nodiscard]] Settings& GetAppSettings()
	{
		return m_appSettings;
	}

	[[nodiscard]] Renderer::RenderSystem& GetRenderSystem()
	{
		return m_renderSystem;
	}

	[[nodiscard]] Platform::Window& GetWindow()
	{
		return m_window;
	}

	[[nodiscard]] World& GetWorld()
	{
		return m_world;
	}

	Dispatcher<> OnApplicationInitialized;

private:

	void Run();

	InputSystem m_inputSystem;
	Settings    m_appSettings;

	bool m_isRunning = false;
	bool m_isPaused  = false;


	World            m_world;
	Platform::Window m_window;

	Renderer::RenderSystem m_renderSystem;
};
