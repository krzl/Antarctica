#include "stdafx.h"
#include "AlignmentBehavior.h"

#include "Entities/MovementComponent.h"
#include "GameObjects/GameObject.h"

namespace Navigation
{
	Vector2D AlignmentBehavior::GetLinearAcceleration()
	{
		if (!m_movement->HasTarget())
		{
			return Vector2D::zero;
		}

		Vector2D heading    = Vector2D::zero;
		uint32_t actorCount = 0;

		for (const MovementComponent* target : GetCachedTargets())
		{
			const float distance = SquaredMag(target->GetOwner()->GetPosition().xy - m_movement->GetOwner()->GetPosition().xy);

			const float cohesionRange = (m_movement->GetRadius() + target->GetRadius()) * m_cohesionScale;

			if (distance < cohesionRange && m_movement->GetVelocity() != Vector2D::zero &&
				target->HasTarget() &&
				Magnitude(target->GetTarget() - target->GetTarget()) < cohesionRange)
			{
				++actorCount;
				heading += Normalize(target->GetVelocity());
			}
		}

		if (actorCount == 0)
		{
			return Vector2D::zero;
		}

		const Vector2D averageHeading = heading / actorCount;

		const Vector2D desiredVelocity = averageHeading * m_movement->GetMaxSpeed();

		const Vector2D velocityChange = desiredVelocity - m_movement->GetVelocity();
		return velocityChange * (m_movement->GetMaxAcceleration() / m_movement->GetMaxSpeed());
	}
}
