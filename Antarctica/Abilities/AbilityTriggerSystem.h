#pragma once

#include "AbilityBinding.h"
#include "AbilityStackComponent.h"
#include "AbilityTriggerComponent.h"
#include "Input/InputListener.h"
#include "Systems/System.h"

class AbilityTriggerSystem : public System<AbilityTriggerComponent, AbilityStackComponent, InputListenerComponent>
{
	void Update(uint64_t         entityId,
		AbilityTriggerComponent* abilityTrigger,
		AbilityStackComponent*   abilityStack,
		InputListenerComponent*  inputListener) override;

	bool IsLockStepSystem() override { return true; }

	static bool IsAbilityTriggered(const AbilityBinding& abilityBinding, const InputCommand& inputCommand);
};
