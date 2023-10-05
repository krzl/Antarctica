#pragma once

#include "Systems/System.h"

struct AbilityStackComponent;

class AbilitySystem : public System<AbilityStackComponent>
{
public:

	AbilitySystem();

private:
	
	void Update(Entity* entity, AbilityStackComponent* abilityStack) override;
};
