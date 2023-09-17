#pragma once

#include "Input/InputCommand.h"

class AbilityActivator;

struct AbilityBinding
{
	std::string m_abilityId;

	InputCommand::Type m_inputType;
	uint32_t m_inputId;

	std::function<std::shared_ptr<AbilityActivator>()> m_activatorCreator = []
	{
		return nullptr;
	};
};
