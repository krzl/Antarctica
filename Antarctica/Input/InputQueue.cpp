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

	const std::pair<int32_t, int32_t> mouseDelta = inputManager->GetMouseDelta();

	if (mouseDelta.first != 0 || mouseDelta.second != 0)
	{
		InputCommand command;

		command.m_type                    = InputCommand::Type::MOUSE_MOVE;
		command.m_mouseMoveInput.m_posX   = inputManager->GetMousePosition().first;
		command.m_mouseMoveInput.m_posY   = inputManager->GetMousePosition().second;
		command.m_mouseMoveInput.m_deltaX = inputManager->GetMouseDelta().first;
		command.m_mouseMoveInput.m_deltaY = inputManager->GetMouseDelta().second;

		m_commands.emplace_back(std::move(command));
	}
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
