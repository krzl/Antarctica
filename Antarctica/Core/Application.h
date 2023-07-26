#pragma once

#include <Renderer.h>
#include <Window.h>
#include <Input/InputManager.h>
#include <Settings/Settings.h>

#include "SystemContainer.h"
#include "Entities/World.h"
#include "Managers/FrameCounter.h"
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

	[[nodiscard]] Settings& GetAppSettings() { return m_appSettings; }
	[[nodiscard]] Rendering::Renderer& GetRenderer() { return m_renderer; }
	[[nodiscard]] Platform::Window& GetWindow() { return m_window; }
	[[nodiscard]] World& GetWorld() { return m_world; }

	template<typename T, class = std::enable_if_t<std::is_base_of_v<SystemBase, T>>>
	T* GetSystem()
	{
		return static_cast<T*>(m_ecs.GetSystem(typeid(T).hash_code()));
	}

	Dispatcher<> OnApplicationInitialized;

private:

	void Run();

	InputManager m_inputManager;
	Settings m_appSettings;

	FrameCounter m_frameCounter = {};

	bool m_isRunning = false;
	bool m_isPaused  = false;

	World m_world;
	Platform::Window m_window;

	Rendering::Renderer m_renderer;

	std::vector<Manager*> m_managers;

	SystemContainer m_ecs;
};
