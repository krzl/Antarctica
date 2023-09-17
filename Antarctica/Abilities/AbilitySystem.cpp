#include "stdafx.h"
#include "AbilitySystem.h"

#include "Ability.h"
#include "AbilityStackComponent.h"

AbilitySystem::AbilitySystem()
{
	m_isMultiThreaded = false;
}

void AbilitySystem::Update(const uint64_t entityId, AbilityStackComponent* abilityStack)
{
	if (abilityStack->m_stack.size() == 0)
	{
		return;
	}
	const std::shared_ptr<Ability>& ability = abilityStack->m_stack.front();

	if (ability->Tick())
	{
		abilityStack->m_stack.pop();
	}
}
