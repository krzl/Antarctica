#pragma once

#include "../Utilities/Math/Point.h"

class InputSystem
{
	friend class InputHandler;
	
public:

	InputSystem();

	bool IsLeftMousePressed() const
	{
		return m_isLeftMousePressed;
	}
	
	bool IsMiddleMousePressed() const
	{
		return m_isMiddleMousePressed;
	}
	bool IsRightMousePressed() const
	{
		return m_isRightMousePressed;
	}
	
	Dispatcher<> OnLeftMouseButtonPressed;
	Dispatcher<> OnMiddleMouseButtonPressed;
	Dispatcher<> OnRightMouseButtonPressed;
				 
	Dispatcher<> OnLeftMouseButtonReleased;
	Dispatcher<> OnMiddleMouseButtonReleased;
	Dispatcher<> OnRightMouseButtonReleased;
	
	Dispatcher<int, int> OnMouseMove;

private:

	bool m_isLeftMousePressed = false;
	bool m_isMiddleMousePressed = false;
	bool m_isRightMousePressed = false;

	PointInt2D m_mousePosition;
};
