#pragma once

#include "Common/Class.h"
#include "Input/InputCommand.h"

struct AbilityBinding
{
	InputCommand::Type m_inputType;
	uint32_t m_inputId;
	const Class* m_abilityClass;
};
