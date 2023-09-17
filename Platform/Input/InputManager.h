#pragma once

#include "Key.h"

#include "Managers/Manager.h"

class InputManager : public Manager
{
	REGISTER_MANAGER(InputManager);

	friend class InputHandler;

public:

	InputManager();

	[[nodiscard]] bool IsLeftMousePressed() const { return m_isLeftMousePressed; }
	[[nodiscard]] bool IsMiddleMousePressed() const { return m_isMiddleMousePressed; }
	[[nodiscard]] bool IsRightMousePressed() const { return m_isRightMousePressed; }
	[[nodiscard]] const Point2DInt& GetMousePosition() const { return m_mousePosition; }
	[[nodiscard]] const Point2DInt& GetMouseDelta() const { return m_mouseDelta; }

	Dispatcher<> OnLeftMouseButtonPressed;
	Dispatcher<> OnMiddleMouseButtonPressed;
	Dispatcher<> OnRightMouseButtonPressed;

	Dispatcher<> OnLeftMouseButtonReleased;
	Dispatcher<> OnMiddleMouseButtonReleased;
	Dispatcher<> OnRightMouseButtonReleased;

	Dispatcher<Key> OnKeyPressed;
	Dispatcher<Key> OnKeyReleased;

	Dispatcher<int, int> OnMouseMove;

	bool IsKeyPressed(Key key) const;

protected:

	void Update() override;

private:

	bool m_isLeftMousePressed   = false;
	bool m_isMiddleMousePressed = false;
	bool m_isRightMousePressed  = false;

	std::bitset<(uint32_t) Key::KEY_ENUM_COUNT> m_heldKeys;

	Point2DInt m_mousePosition;
	Point2DInt m_oldMousePosition;

	Point2DInt m_mouseDelta = { 0, 0 };
};
