#pragma once

#include <Window.h>
#include <Input/InputSystem.h>
#include <Settings/Settings.h>

class Application
{
public:
	
	void Start();
	void Stop();

	void Pause();
	void Unpause();

	void Update();
	void Render();

	InputSystem m_inputSystem;
	Settings m_appSettings;

private:
	
	void Run();

	bool m_isRunning = false;
	bool m_isPaused = false;

	Platform::Window m_window;
};
