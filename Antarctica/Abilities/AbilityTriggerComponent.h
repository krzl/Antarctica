#pragma once

#include "AbilityBinding.h"
#include "Components/Component.h"

struct AbilityTriggerComponent : Component
{
	std::vector<AbilityBinding> m_abilityBindings;

	bool m_alwaysAppendAbilities = false;

	DEFINE_CLASS();
};

CREATE_CLASS(AbilityTriggerComponent)
