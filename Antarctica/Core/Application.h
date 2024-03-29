#pragma once

#include <Renderer.h>
#include <Window.h>
#include <Input/InputManager.h>
#include <Settings/Settings.h>

#include "ECS.h"
#include "Entities/World.h"

#include "Game/GameState.h"

#include "Managers/FrameCounter.h"
#include "Systems/RenderSystem.h"

class PlayerCameraSystem;

class SystemBase;
class Manager;
class World;

class Application
{
public:

	Application() {}

	void CreateSystems();
	void Start();
	void Stop();

	void Pause();
	void Unpause();

	static Application& Get();

	[[nodiscard]] Settings& GetAppSettings() { return m_appSettings; }
	[[nodiscard]] Rendering::Renderer& GetRenderer() { return m_renderer; }
	[[nodiscard]] Platform::Window& GetWindow() { return m_window; }
	[[nodiscard]] World& GetWorld() { return m_world; }
	[[nodiscard]] GameState& GetGameState() { return m_gameState; }

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

	GameState m_gameState;

	Rendering::Renderer m_renderer;

	std::vector<Manager*> m_managers;

	ECS m_ecs;
};
