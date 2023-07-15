#include "stdafx.h"
#include "SteeringBehavior.h"

#include "Entities/MovementComponent.h"

namespace Navigation
{
	SteeringBehavior::SteeringBehavior(MovementComponent* movementComponent)
		: m_movement(movementComponent) { }

	const std::vector<MovementComponent*>& SteeringBehavior::GetCachedTargets() const
	{
		return m_movement->m_targets;
	}
}
