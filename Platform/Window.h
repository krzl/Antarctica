#pragma once
#include "Input/InputHandler.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class InputManager;
class Settings;

namespace Platform
{
	class Window
	{
		friend LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	public:

		void Init(const Settings& appSettings);
		void SetupInputManager(InputManager& inputManager);
		void Update();

		Dispatcher<> OnDestroyed;

		[[nodiscard]] HWND GetHandle() const
		{
			return m_handle;
		}

		[[nodiscard]] float GetAspectRatio() const
		{
			return static_cast<float>(m_width) / static_cast<float>(m_height);
		}

		[[nodiscard]] uint32_t GetWidth() const
		{
			return m_width;
		}

		[[nodiscard]] uint32_t GetHeight() const
		{
			return m_height;
		}

		Dispatcher<> OnResized;
		
	private:

		LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		HWND m_handle = nullptr;

		bool m_hasFocus    = true;
		bool m_isMinimized = false;
		bool m_isMaximized = false;
		bool m_isResizing  = false;

		bool m_hasResizedLastFrame = false;

		uint32_t m_width  = 0;
		uint32_t m_height = 0;

		InputHandler m_inputHandler;
	};
}
