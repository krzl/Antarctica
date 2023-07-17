#pragma once

#include "Managers/Manager.h"

class InputManager : public Manager
{
	REGISTER_MANAGER(InputManager);

	friend class InputHandler;

public:

	InputManager();

	[[nodiscard]] bool IsLeftMousePressed() const
	{
		return m_isLeftMousePressed;
	}

	[[nodiscard]] bool IsMiddleMousePressed() const
	{
		return m_isMiddleMousePressed;
	}

	[[nodiscard]] bool IsRightMousePressed() const
	{
		return m_isRightMousePressed;
	}

	[[nodiscard]] const MousePosition& GetMousePosition() const
	{
		return m_mousePosition;
	}

	[[nodiscard]] const std::pair<int32_t, int32_t>& GetMouseDelta() const
	{
		return m_mouseDelta;
	}

	Dispatcher<> OnLeftMouseButtonPressed;
	Dispatcher<> OnMiddleMouseButtonPressed;
	Dispatcher<> OnRightMouseButtonPressed;

	Dispatcher<> OnLeftMouseButtonReleased;
	Dispatcher<> OnMiddleMouseButtonReleased;
	Dispatcher<> OnRightMouseButtonReleased;

	Dispatcher<int, int> OnMouseMove;

protected:

	void Update() override;

private:

	bool m_isLeftMousePressed   = false;
	bool m_isMiddleMousePressed = false;
	bool m_isRightMousePressed  = false;

	MousePosition m_mousePosition;
	MousePosition m_oldMousePosition;

	std::pair<int32_t, int32_t> m_mouseDelta = std::pair(0, 0);
};
