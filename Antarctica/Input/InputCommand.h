#pragma once

struct InputCommand
{
	//TODO: add keyboard press/elease
	enum class Type
	{
		MOUSE_PRESS,
		MOUSE_RELEASE,
		MOUSE_MOVE,
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
		uint32_t m_posX,   m_posY;
		int32_t  m_deltaX, m_deltaY;
	};

	union
	{
		MousePressInput   m_mousePressInput;
		MouseReleaseInput m_mouseReleaseInput;
		MouseMoveInput    m_mouseMoveInput;
	};
};
