#pragma once

#include "AbilityStackComponent.h"
#include "AbilityTriggerComponent.h"
#include "Systems/System.h"

struct InputCommand;
struct AbilityBinding;
struct InputListenerComponent;

class AbilityTriggerSystem : public System<AbilityTriggerComponent, AbilityStackComponent, InputListenerComponent>
{
	void Update(uint64_t entityId, AbilityTriggerComponent* abilityTrigger, AbilityStackComponent* abilityStack,
				InputListenerComponent* inputListener) override;

	static bool IsAbilityTriggered(const AbilityBinding& abilityBinding, const InputCommand& inputCommand);
};
