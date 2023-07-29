#include "stdafx.h"
#include "InputHandler.h"

#include <WindowsX.h>

#include "InputManager.h"

void InputHandler::Init(InputManager& inputManager)
{
	m_inputManager = &inputManager;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void InputHandler::ProcessMessage(const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
	if (!m_inputManager)
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
	const int32_t x = GET_X_LPARAM(lParam);
	const int32_t y = GET_Y_LPARAM(lParam);

	m_inputManager->m_mousePosition = Point2DInt{ x, y };

	switch (msg)
	{
		case WM_LBUTTONDOWN:
			m_inputManager->OnLeftMouseButtonPressed.Dispatch();
			return;
		case WM_MBUTTONDOWN:
			m_inputManager->OnMiddleMouseButtonPressed.Dispatch();
			return;
		case WM_RBUTTONDOWN:
			m_inputManager->OnRightMouseButtonPressed.Dispatch();
			return;
		case WM_LBUTTONUP:
			m_inputManager->OnLeftMouseButtonReleased.Dispatch();
			return;
		case WM_MBUTTONUP:
			m_inputManager->OnMiddleMouseButtonReleased.Dispatch();
			return;
		case WM_RBUTTONUP:
			m_inputManager->OnRightMouseButtonReleased.Dispatch();
			return;
		case WM_MOUSEMOVE:
			m_inputManager->OnMouseMove.Dispatch(x, y);
			return;
	}
}
