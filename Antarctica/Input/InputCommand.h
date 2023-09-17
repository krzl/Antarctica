#pragma once

#include "Input/Key.h"

struct InputCommand
{
	enum class Type
	{
		MOUSE_PRESS,
		MOUSE_RELEASE,
		MOUSE_MOVE,
		KEY_PRESS,
		KEY_RELEASE
	} m_type;

	enum class MouseButtonId
	{
		LEFT,
		RIGHT,
		MIDDLE
	};

	struct MousePressInput
	{
		MouseButtonId m_button;
	};

	struct MouseReleaseInput
	{
		MouseButtonId m_button;
	};

	struct MouseMoveInput
	{
		uint32_t m_posX, m_posY;
		int32_t m_deltaX, m_deltaY;
	};

	struct KeyPressInput
	{
		Key m_button;
	};

	struct KeyReleaseInput
	{
		Key m_button;
	};

	union
	{
		MousePressInput m_mousePressInput;
		MouseReleaseInput m_mouseReleaseInput;
		MouseMoveInput m_mouseMoveInput;
		KeyPressInput m_keyPressInput;
		KeyReleaseInput m_keyReleaseInput;
	};
};
