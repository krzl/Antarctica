#pragma once

#include "Components/Component.h"

struct AbilityBinding;

struct AbilityTriggerComponent : Component
{
	std::vector<AbilityBinding> m_abilityBindings;

	bool m_alwaysAppendAbilities = false;

	DEFINE_CLASS();
};

CREATE_CLASS(AbilityTriggerComponent)
