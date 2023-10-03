#pragma once

#include "AbilityActivator.h"

class InstantMoveActivator : public AbilityActivator
{
public:

	InstantMoveActivator();

	void Update() override;

	bool CanBeFinished() override;
	bool ShouldBeCancelled() override;

	std::shared_ptr<Ability> Activate(Entity* entity) override;
	void OnFinished() override;

private:

	std::optional<Point3D> m_cursorPosition;

	uint32_t m_entityActivationCount = 0;
	
	uint32_t NAVIGATION_MAX_UNITS_PER_FRAME = 64;
};
