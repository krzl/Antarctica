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

	OnKeyPressed.AddListener([this](const Key key)
	{
		m_heldKeys.set((uint32_t) key, true);
	});

	OnKeyReleased.AddListener([this](const Key key)
	{
		m_heldKeys.set((uint32_t) key, false);
	});
}

bool InputManager::IsKeyPressed(const Key key) const
{
	return m_heldKeys.test((uint32_t) key);
}

void InputManager::Update()
{
	m_mouseDelta = Point2DInt{
		m_mousePosition.x - m_oldMousePosition.x,
		m_mousePosition.y - m_oldMousePosition.y
	};
	m_oldMousePosition = m_mousePosition;
}
