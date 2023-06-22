#include "stdafx.h"
#include "InputSystem.h"

InputSystem::InputSystem()
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

void InputSystem::Update()
{
	m_mouseDelta = std::pair(m_mousePosition.first - m_oldMousePosition.first,
							 m_mousePosition.second - m_oldMousePosition.second);
	m_oldMousePosition = m_mousePosition;
}
