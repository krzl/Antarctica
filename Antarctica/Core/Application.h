#pragma once

#include <Renderer.h>
#include <Window.h>
#include <Input/InputManager.h>
#include <Settings/Settings.h>

#include "Entities/World.h"
#include "Input/InputSystem.h"
#include "Managers/FrameCounter.h"
#include "Managers/TimeManager.h"
#include "Systems/RenderSystem.h"

class PlayerCameraSystem;

class SystemBase;
class Manager;
class World;

class Application
{
public:

	void Start();
	void Stop();

	void Pause();
	void Unpause();

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
	Settings     m_appSettings;

	FrameCounter m_frameCounter = {};

	bool m_isRunning = false;
	bool m_isPaused  = false;

	std::vector<SystemBase*> m_preStepLockSystems;
	std::vector<SystemBase*> m_stepLockSystems;
	std::vector<SystemBase*> m_postStepLockSystems;

	World            m_world;
	Platform::Window m_window;

	InputSystem*        m_inputSystem        = nullptr;
	PlayerCameraSystem* m_playerCameraSystem = nullptr;

	Rendering::Renderer      m_renderer;
	Rendering::RenderSystem* m_renderSystem = nullptr;

	std::vector<Manager*> m_managers;
};
