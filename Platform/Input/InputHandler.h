#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class InputManager;

class InputHandler
{
public:

	void Init(InputManager& inputManager);
	void ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam);

private:

	void ProcessMouseInput(UINT msg, WPARAM wParam, LPARAM lParam) const;

	InputManager* m_inputManager = nullptr;
};
