#include "stdafx.h"
#include "InputQueue.h"

#include "Input/InputManager.h"


void InputQueue::AddMousePressCommand(const InputCommand::MouseButtonId mouseButtonId, const bool isPressed)
{
	InputCommand command;
	if (isPressed)
	{
		command.m_type                     = InputCommand::Type::MOUSE_PRESS;
		command.m_mousePressInput.m_button = mouseButtonId;
	}
	else
	{
		command.m_type                       = InputCommand::Type::MOUSE_RELEASE;
		command.m_mouseReleaseInput.m_button = mouseButtonId;
	}

	m_commands.emplace_back(std::move(command));
}

void InputQueue::TryAddMouseMoveCommand()
{
	const InputManager* inputManager = InputManager::GetInstance();

	const Point2DInt mouseDelta = inputManager->GetMouseDelta();

	if (mouseDelta.x != 0 || mouseDelta.y != 0)
	{
		InputCommand command;

		command.m_type                    = InputCommand::Type::MOUSE_MOVE;
		command.m_mouseMoveInput.m_posX   = inputManager->GetMousePosition().x;
		command.m_mouseMoveInput.m_posY   = inputManager->GetMousePosition().y;
		command.m_mouseMoveInput.m_deltaX = inputManager->GetMouseDelta().x;
		command.m_mouseMoveInput.m_deltaY = inputManager->GetMouseDelta().y;

		m_commands.emplace_back(std::move(command));
	}
}

void InputQueue::AddKeyPressCommand(const Key key, const bool isPressed)
{
	InputCommand command;
	if (isPressed)
	{
		command.m_type                   = InputCommand::Type::KEY_PRESS;
		command.m_keyPressInput.m_button = key;
	}
	else
	{
		command.m_type                     = InputCommand::Type::KEY_RELEASE;
		command.m_keyReleaseInput.m_button = key;
	}

	m_commands.emplace_back(std::move(command));
}

void InputQueue::Clear()
{
	m_commands.clear();
}

const InputCommand::MousePressInput* InputQueue::GetMouseButtonPress(const InputCommand::MouseButtonId mouseButtonId) const
{
	for (const InputCommand& command : m_commands)
	{
		if (command.m_type == InputCommand::Type::MOUSE_PRESS && command.m_mousePressInput.m_button == mouseButtonId)
		{
			return &command.m_mousePressInput;
		}
	}

	return nullptr;
}

const InputCommand::MouseReleaseInput* InputQueue::GetMouseButtonRelease(const InputCommand::MouseButtonId mouseButtonId) const
{
	for (const InputCommand& command : m_commands)
	{
		if (command.m_type == InputCommand::Type::MOUSE_RELEASE && command.m_mouseReleaseInput.m_button == mouseButtonId)
		{
			return &command.m_mouseReleaseInput;
		}
	}

	return nullptr;
}

const InputCommand::MouseMoveInput* InputQueue::GetMouseMove() const
{
	for (const InputCommand& command : m_commands)
	{
		if (command.m_type == InputCommand::Type::MOUSE_MOVE)
		{
			return &command.m_mouseMoveInput;
		}
	}

	return nullptr;
}

const InputCommand::MousePressInput* InputQueue::GetKeyPress(const Key key) const
{
	for (const InputCommand& command : m_commands)
	{
		if (command.m_type == InputCommand::Type::MOUSE_PRESS && command.m_keyPressInput.m_button == key)
		{
			return &command.m_mousePressInput;
		}
	}

	return nullptr;
}

const InputCommand::MouseReleaseInput* InputQueue::GetKeyRelease(const Key key) const
{
	for (const InputCommand& command : m_commands)
	{
		if (command.m_type == InputCommand::Type::MOUSE_RELEASE && command.m_keyReleaseInput.m_button == key)
		{
			return &command.m_mouseReleaseInput;
		}
	}

	return nullptr;
}
