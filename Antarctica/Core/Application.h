#pragma once

#include <Renderer.h>
#include <Window.h>
#include <Input/InputManager.h>
#include <Settings/Settings.h>

#include "GameObjects/World.h"
#include "Managers/TimeManager.h"

class Manager;
class World;

class Application
{
public:

	void Start();
	void Stop();

	void Pause();
	void Unpause();

	void Update(float deltaTime);

	static Application& Get();

	[[nodiscard]] Settings& GetAppSettings()
	{
		return m_appSettings;
	}

	[[nodiscard]] Rendering::Renderer& GetRenderer()
	{
		return m_renderer;
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

	InputManager m_inputManager;
	Settings    m_appSettings;

	bool m_isRunning = false;
	bool m_isPaused  = false;


	World            m_world;
	Platform::Window m_window;

	Rendering::Renderer m_renderer;

	std::vector<Manager*> m_managers;
};
