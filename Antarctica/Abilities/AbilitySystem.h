#pragma once

#include "Systems/System.h"

struct AbilityStackComponent;
struct InputListenerComponent;

class AbilitySystem : public System<AbilityStackComponent, InputListenerComponent>
{
	void Update(uint64_t entityId, AbilityStackComponent* abilityStack, InputListenerComponent* inputListener) override;

	bool IsLockStepSystem() override { return true; }
};
