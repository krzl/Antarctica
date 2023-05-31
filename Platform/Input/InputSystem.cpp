#include "stdafx.h"
#include "InputSystem.h"

InputSystem::InputSystem()
{
	OnLeftMouseButtonPressed.AddListener([this]()
	{
		m_isLeftMousePressed = true;
	}, false);

	OnMiddleMouseButtonPressed.AddListener([this]()
	{
		m_isMiddleMousePressed = true;
	}, false);

	OnRightMouseButtonPressed.AddListener([this]()
	{
		m_isRightMousePressed = true;
	}, false);

	OnLeftMouseButtonReleased.AddListener([this]()
	{
		m_isLeftMousePressed = false;
	}, false);

	OnMiddleMouseButtonReleased.AddListener([this]()
	{
		m_isMiddleMousePressed = false;
	}, false);

	OnRightMouseButtonReleased.AddListener([this]()
	{
		m_isRightMousePressed = false;
	}, false);
}

void InputSystem::Update()
{
	m_mouseDelta = std::pair(m_mousePosition.first - m_oldMousePosition.first,
							 m_mousePosition.second - m_oldMousePosition.second);
	m_oldMousePosition = m_mousePosition;
}
