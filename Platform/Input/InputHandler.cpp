#include "stdafx.h"
#include "InputHandler.h"

#include <WindowsX.h>

#include "InputManager.h"
#include "Key.h"

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
		case WM_MOUSEWHEEL:
			ProcessMouseInput(msg, wParam, lParam);
			return;
		case WM_KEYDOWN:
			HandleInputPress(wParam);
			return;
		case WM_KEYUP:
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
				return;
			}
			HandleInputRelease(wParam);
			return;
	}
}

void InputHandler::ProcessMouseInput(const UINT msg, const WPARAM wParam, const LPARAM lParam) const
{
	const int32_t x = GET_X_LPARAM(lParam);
	const int32_t y = GET_Y_LPARAM(lParam);

	m_inputManager->m_mousePosition   = Point2DInt{ x, y };
	
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
		case WM_MOUSEWHEEL:
			m_inputManager->m_mouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			return;
	}
}

static Key TranslateKeyCode(const WPARAM keycode)
{
	switch (keycode)
	{
		case VK_BACK:
			return Key::BACKSPACE;
		case VK_TAB:
			return Key::TAB;
		case VK_RETURN:
			return Key::ENTER;
		case VK_CAPITAL:
			return Key::CAPS_LOCK;
		case VK_ESCAPE:
			return Key::ESCAPE;
		case VK_SPACE:
			return Key::SPACE;
		case VK_PRIOR:
			return Key::PAGE_UP;
		case VK_NEXT:
			return Key::PAGE_DOWN;
		case VK_END:
			return Key::END;
		case VK_HOME:
			return Key::HOME;
		case VK_LEFT:
			return Key::LEFT_ARROW;
		case VK_UP:
			return Key::UP_ARROW;
		case VK_RIGHT:
			return Key::RIGHT_ARROW;
		case VK_DOWN:
			return Key::DOWN_ARROW;
		case VK_INSERT:
			return Key::INSERT;
		case VK_DELETE:
			return Key::DELETE;
		case 0x30:
			return Key::DIGIT_0;
		case 0x31:
			return Key::DIGIT_1;
		case 0x32:
			return Key::DIGIT_2;
		case 0x33:
			return Key::DIGIT_3;
		case 0x34:
			return Key::DIGIT_4;
		case 0x35:
			return Key::DIGIT_5;
		case 0x36:
			return Key::DIGIT_6;
		case 0x37:
			return Key::DIGIT_7;
		case 0x38:
			return Key::DIGIT_8;
		case 0x39:
			return Key::DIGIT_9;
		case 0x41:
			return Key::A;
		case 0x42:
			return Key::B;
		case 0x43:
			return Key::C;
		case 0x44:
			return Key::D;
		case 0x45:
			return Key::E;
		case 0x46:
			return Key::F;
		case 0x47:
			return Key::G;
		case 0x48:
			return Key::H;
		case 0x49:
			return Key::I;
		case 0x4A:
			return Key::J;
		case 0x4B:
			return Key::K;
		case 0x4C:
			return Key::L;
		case 0x4D:
			return Key::M;
		case 0x4E:
			return Key::N;
		case 0x4F:
			return Key::O;
		case 0x50:
			return Key::P;
		case 0x51:
			return Key::Q;
		case 0x52:
			return Key::R;
		case 0x53:
			return Key::S;
		case 0x54:
			return Key::T;
		case 0x55:
			return Key::U;
		case 0x56:
			return Key::V;
		case 0x57:
			return Key::W;
		case 0x58:
			return Key::X;
		case 0x59:
			return Key::Y;
		case 0x5A:
			return Key::Z;
		case VK_NUMPAD0:
			return Key::NUM_0;
		case VK_NUMPAD1:
			return Key::NUM_1;
		case VK_NUMPAD2:
			return Key::NUM_2;
		case VK_NUMPAD3:
			return Key::NUM_3;
		case VK_NUMPAD4:
			return Key::NUM_4;
		case VK_NUMPAD5:
			return Key::NUM_5;
		case VK_NUMPAD6:
			return Key::NUM_6;
		case VK_NUMPAD7:
			return Key::NUM_7;
		case VK_NUMPAD8:
			return Key::NUM_8;
		case VK_NUMPAD9:
			return Key::NUM_9;
		case VK_MULTIPLY:
			return Key::NUM_ASTERISK;
		case VK_ADD:
			return Key::NUM_PLUS;
		case VK_SUBTRACT:
			return Key::NUM_MINUS;
		case VK_DECIMAL:
			return Key::NUM_PERIOD;
		case VK_DIVIDE:
			return Key::NUM_SLASH;
		case VK_F1:
			return Key::F1;
		case VK_F2:
			return Key::F2;
		case VK_F3:
			return Key::F3;
		case VK_F4:
			return Key::F4;
		case VK_F5:
			return Key::F5;
		case VK_F6:
			return Key::F6;
		case VK_F7:
			return Key::F7;
		case VK_F8:
			return Key::F8;
		case VK_F9:
			return Key::F9;
		case VK_F10:
			return Key::F10;
		case VK_F11:
			return Key::F11;
		case VK_F12:
			return Key::F12;
		case VK_NUMLOCK:
			return Key::NUM_LOCK;
		case VK_SHIFT:
			return Key::SHIFT;
		case VK_CONTROL:
			return Key::CTRL;
		case VK_MENU:
			return Key::ALT;
		case VK_OEM_MINUS:
			return Key::MINUS;
		case VK_OEM_PLUS:
			return Key::PLUS;
		case VK_OEM_3:
			return Key::APOSTROPHE;
		case VK_OEM_4:
			return Key::LEFT_BRACKET;
		case VK_OEM_6:
			return Key::RIGHT_BRACKET;
		case VK_OEM_5:
			return Key::BACKSLASH;
		case VK_OEM_1:
			return Key::SEMICOLON;
		case VK_OEM_7:
			return Key::QUOTE;
		case VK_OEM_COMMA:
			return Key::COMMA;
		case VK_OEM_PERIOD:
			return Key::PERIOD;
		case VK_OEM_2:
			return Key::SLASH;
		default:
			return Key::INVALID_KEY;
	}
}

void InputHandler::HandleInputPress(const WPARAM keycode) const
{
	const Key key = TranslateKeyCode(keycode);
	if (key != Key::INVALID_KEY)
	{
		m_inputManager->OnKeyPressed.Dispatch(key);
	}
}

void InputHandler::HandleInputRelease(const WPARAM keycode) const
{
	const Key key = TranslateKeyCode(keycode);
	if (key != Key::INVALID_KEY)
	{
		m_inputManager->OnKeyReleased.Dispatch(key);
	}
}
