#pragma once

#include "Systems/System.h"

struct AbilityStackComponent;

class AbilitySystem : public System<AbilityStackComponent>
{
public:

	AbilitySystem();

private:
	
	void Update(uint64_t entityId, AbilityStackComponent* abilityStack) override;
};
