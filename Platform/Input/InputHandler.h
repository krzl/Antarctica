#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class InputSystem;

class InputHandler
{
public:

	void Init(InputSystem& inputSystem);
	void ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam);

private:

	void ProcessMouseInput(UINT msg, WPARAM wParam, LPARAM lParam) const;

	InputSystem* m_inputSystem = nullptr;
};
