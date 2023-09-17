#pragma once

#include "InputCommand.h"
#include "Input/Key.h"

class InputQueue
{
public:

	std::vector<InputCommand> m_commands;

	void AddMousePressCommand(InputCommand::MouseButtonId mouseButtonId, bool isPressed);
	void TryAddMouseMoveCommand();
	void AddKeyPressCommand(Key key, bool isPressed);

	void Clear();

	const InputCommand::MousePressInput* GetMouseButtonPress(InputCommand::MouseButtonId mouseButtonId) const;
	const InputCommand::MouseReleaseInput* GetMouseButtonRelease(InputCommand::MouseButtonId mouseButtonId) const;
	const InputCommand::MouseMoveInput* GetMouseMove() const;
	const InputCommand::MousePressInput* GetKeyPress(Key key) const;
	const InputCommand::MouseReleaseInput* GetKeyRelease(Key key) const;
};
