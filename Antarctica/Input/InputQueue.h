#pragma once

#include "InputCommand.h"

class InputQueue
{
public:

	std::vector<InputCommand> m_commands;

	void AddMousePressCommand(InputCommand::MouseButtonId mouseButtonId, bool isPressed);
	void TryAddMouseMoveCommand();
	void Clear();

	const InputCommand::MousePressInput*   GetMouseButtonPress(InputCommand::MouseButtonId mouseButtonId) const;
	const InputCommand::MouseReleaseInput* GetMouseButtonRelease(InputCommand::MouseButtonId mouseButtonId) const;
	const InputCommand::MouseMoveInput*    GetMouseMove() const;
};
