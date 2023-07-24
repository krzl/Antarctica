#pragma once

#include "Entities/Entity.h"

class Character : public Entity
{
	void DefineArchetype(ArchetypeBuilder& builder) override;
	void SetupComponents(ComponentAccessor& accessor) override;
};
