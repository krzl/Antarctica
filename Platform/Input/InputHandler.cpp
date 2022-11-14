#include "stdafx.h"
#include "InputHandler.h"

#include <WindowsX.h>

#include "InputSystem.h"

void InputHandler::Init(InputSystem& inputSystem)
{
	m_inputSystem = &inputSystem;
}

void InputHandler::ProcessMessage(const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
	if (!m_inputSystem)
		return;

	switch (msg)
	{
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
			ProcessMouseInput(msg, wParam, lParam);
			return;
		case WM_KEYUP:
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
			}
			return;
	}
}
void InputHandler::ProcessMouseInput(const UINT msg, const WPARAM wParam, const LPARAM lParam) const
{
	const int32_t x = static_cast<int32_t>(GET_X_LPARAM(lParam));
	const int32_t y = static_cast<int32_t>(GET_Y_LPARAM(lParam));

	m_inputSystem->m_mousePosition = MousePosition(x, y);

	switch (msg)
	{
		case WM_LBUTTONDOWN:
			m_inputSystem->OnLeftMouseButtonPressed.Dispatch();
			return;
		case WM_MBUTTONDOWN:
			m_inputSystem->OnMiddleMouseButtonPressed.Dispatch();
			return;
		case WM_RBUTTONDOWN:
			m_inputSystem->OnRightMouseButtonPressed.Dispatch();
			return;
		case WM_LBUTTONUP:
			m_inputSystem->OnLeftMouseButtonReleased.Dispatch();
			return;
		case WM_MBUTTONUP:
			m_inputSystem->OnMiddleMouseButtonReleased.Dispatch();
			return;
		case WM_RBUTTONUP:
			m_inputSystem->OnRightMouseButtonReleased.Dispatch();
			return;
		case WM_MOUSEMOVE:
			m_inputSystem->OnMouseMove.Dispatch(x, y);
			return;
	}
}
