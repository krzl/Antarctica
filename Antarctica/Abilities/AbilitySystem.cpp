#include "stdafx.h"
#include "AbilitySystem.h"

#include "Ability.h"
#include "AbilityStackComponent.h"
#include "Entities/World.h"
#include "Input/InputListener.h"

void AbilitySystem::Update(const uint64_t entityId, AbilityStackComponent* abilityStack, InputListenerComponent* inputListener)
{
	if (abilityStack->m_stack.size() == 0)
	{
		return;
	}
	const std::shared_ptr<Ability>& ability = abilityStack->m_stack.front();

	Entity* entity = *World::Get()->GetEntity(entityId);

	if (ability->Run(*entity))
	{
		abilityStack->m_stack.pop();
	}
}
