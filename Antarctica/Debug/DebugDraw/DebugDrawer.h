#pragma once

#include "Entities/Entity.h"

class DebugDrawer : public Entity
{
	void DefineArchetype(ArchetypeBuilder& builder) override;
	void SetupComponents(const ComponentAccessor& accessor) override;
};
