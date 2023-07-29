#include "stdafx.h"
#include "InputManager.h"

InputManager::InputManager()
{
	OnLeftMouseButtonPressed.AddListener([this]()
	{
		m_isLeftMousePressed = true;
	});

	OnMiddleMouseButtonPressed.AddListener([this]()
	{
		m_isMiddleMousePressed = true;
	});

	OnRightMouseButtonPressed.AddListener([this]()
	{
		m_isRightMousePressed = true;
	});

	OnLeftMouseButtonReleased.AddListener([this]()
	{
		m_isLeftMousePressed = false;
	});

	OnMiddleMouseButtonReleased.AddListener([this]()
	{
		m_isMiddleMousePressed = false;
	});

	OnRightMouseButtonReleased.AddListener([this]()
	{
		m_isRightMousePressed = false;
	});
}

void InputManager::Update()
{
	m_mouseDelta = Point2DInt{
		m_mousePosition.x - m_oldMousePosition.x,
		m_mousePosition.y - m_oldMousePosition.y
	};
	m_oldMousePosition = m_mousePosition;
}
