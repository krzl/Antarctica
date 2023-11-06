// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr
#include "stdafx.h"
#include "Window.h"

#include "../Utilities/Settings/Settings.h"

#include <exception>
#include <WindowsX.h>

Platform::Window* windowPtr = nullptr;

// ReSharper disable CppParameterMayBeConst
LRESULT CALLBACK Platform::MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
// ReSharper restore CppParameterMayBeConst
{
	return windowPtr->MsgProc(hwnd, msg, wParam, lParam);
}

void Platform::Window::Init(const Settings& appSettings)
{
	windowPtr = this;

	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = GetModuleHandle(NULL);
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = "MainWndClass";

	if (!RegisterClass(&wc))
	{
		throw std::exception("RegisterClass Failed.");
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT rect = {
		0,
		0,
		(LONG) appSettings.GetInt("window.width").value(),
		(LONG) appSettings.GetInt("window.height").value()
	};

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	const int windowWidth  = rect.right - rect.left;
	const int windowHeight = rect.bottom - rect.top;

	m_handle = CreateWindow("MainWndClass", appSettings.GetString("window.title").value().c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		windowWidth, windowHeight, 0, 0, wc.hInstance, 0);
	if (!m_handle)
	{
		throw std::exception("CreateWindow Failed.");
	}

	ShowWindow(m_handle, SW_SHOW);
	UpdateWindow(m_handle);
}

void Platform::Window::SetupInputManager(InputManager& inputManager)
{
	m_inputHandler.Init(inputManager);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void Platform::Window::Update()
{
	MSG msg = { nullptr };

	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT Platform::Window::MsgProc(const HWND hwnd, const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
	switch (msg)
	{
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				m_hasFocus = false;
			}
			else
			{
				m_hasFocus = true;
			}
			return 0;

		case WM_SIZE:
			m_width = LOWORD(lParam);
			m_height = HIWORD(lParam);
			if (wParam == SIZE_MINIMIZED)
			{
				m_hasFocus    = false;
				m_isMinimized = true;
				m_isMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_hasFocus    = true;
				m_isMinimized = false;
				m_isMaximized = true;
				OnResized.Dispatch();
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (m_isMinimized)
				{
					m_hasFocus    = true;
					m_isMinimized = false;
					OnResized.Dispatch();
				}

				else if (m_isMaximized)
				{
					m_hasFocus    = true;
					m_isMaximized = false;
					OnResized.Dispatch();
				}
				else if (m_isResizing)
				{
					// If user is dragging the resize bars, we do not resize 
				}
				else
				{
					OnResized.Dispatch();
				}
			}
			return 0;

		case WM_ENTERSIZEMOVE:
			m_hasFocus = false;
			m_isResizing = true;
			return 0;

		case WM_EXITSIZEMOVE:
			m_hasFocus = true;
			m_isResizing = false;
			OnResized.Dispatch();
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			OnDestroyed.Dispatch();
			return 0;

		case WM_MENUCHAR:
			return MAKELRESULT(0, MNC_CLOSE);

		case WM_GETMINMAXINFO:
			((MINMAXINFO*) lParam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*) lParam)->ptMinTrackSize.y = 200;
			return 0;

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_MOUSEWHEEL:
			m_inputHandler.ProcessMessage(msg, wParam, lParam);
			return 0;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}
