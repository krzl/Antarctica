#pragma once

#include "Components/Component.h"

class Ability;

struct AbilityStackComponent : Component
{
	std::queue<std::shared_ptr<Ability>> m_stack;

	DEFINE_CLASS();
};

CREATE_CLASS(AbilityStackComponent)
