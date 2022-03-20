#pragma once
#include "Input/InputHandler.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class InputSystem;
class Settings;

namespace Platform
{
	class Window
	{
		friend LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	public:

		void Init(InputSystem& inputSystem, const Settings& appSettings);
		void Update();

		Dispatcher<> OnWindowDestroyed;
		
		float GetAspectRatio() const;
		
	private:

		LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		HWND m_handle = nullptr;

		bool m_hasFocus = true;
		bool m_isMinimized = false;
		bool m_isMaximized = false;
		bool m_isResizing = false;

		bool m_hasResizedLastFrame = false;
		
		uint32_t m_width = 0;
		uint32_t m_height = 0;

		InputHandler inputHandler;
	};
}
