#pragma once

#include "Systems/System.h"

struct AbilityStackComponent;
struct InputListenerComponent;

class AbilitySystem : public System<AbilityStackComponent, InputListenerComponent>
{
public:

	AbilitySystem();

private:
	
	void Update(uint64_t entityId, AbilityStackComponent* abilityStack, InputListenerComponent* inputListener) override;
};
