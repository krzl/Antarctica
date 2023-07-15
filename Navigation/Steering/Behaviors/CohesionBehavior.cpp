#include "stdafx.h"
#include "CohesionBehavior.h"

#include "ArriveBehavior.h"
#include "Entities/MovementComponent.h"
#include "GameObjects/GameObject.h"

namespace Navigation
{
	Vector2D CohesionBehavior::GetLinearAcceleration()
	{
		if (!m_movement->HasTarget())
		{
			return Vector2D::zero;
		}
		Vector2D heading = Vector2D::zero;

		uint32_t actorCount = 0;

		for (const MovementComponent* target : GetCachedTargets())
		{
			const float distanceSqr   = SquaredMag(target->GetOwner()->GetPosition().xy - m_movement->GetOwner()->GetPosition().xy);
			const float cohesionRange = (m_movement->GetRadius() + target->GetRadius()) * m_cohesionScale;

			if (distanceSqr < cohesionRange * cohesionRange && m_movement->GetVelocity() != Vector2D::zero &&
				target->HasTarget() &&
				SquaredMag(m_movement->GetTarget() - target->GetTarget()) < cohesionRange * cohesionRange)
			{
				++actorCount;
				heading += target->GetOwner()->GetPosition().xy;
			}
		}

		if (actorCount == 0 || heading == Vector2D::zero)
		{
			return Vector2D::zero;
		}

		const Vector2D target = heading / actorCount;

		const Vector2D direction = target - m_movement->GetOwner()->GetPosition().xy;

		const Vector2D targetVelocity = Normalize(direction) * m_movement->GetMaxSpeed();
		return (targetVelocity - m_movement->GetVelocity()) * (m_movement->GetMaxAcceleration() / m_movement->GetMaxSpeed());
	}
}
