#include "stdafx.h"
#include "AlignmentBehavior.h"

#include "ArriveBehavior.h"
#include "Components/MovementComponent.h"
#include "Components/TransformComponent.h"
#include "Steering/SteeringSystem.h"

namespace Navigation
{
	Vector2D AlignmentBehavior::GetLinearAcceleration(const TransformComponent* transform,
		const MovementComponent*                                                movement,
		const std::vector<NearbyTarget>&                                        nearbyTargets)
	{
		if (!movement->m_arriveBehavior.HasTarget())
		{
			return Vector2D::zero;
		}

		Vector2D heading    = Vector2D::zero;
		uint32_t actorCount = 0;

		for (const NearbyTarget& target : nearbyTargets)
		{
			const float distance = SquaredMag(target.m_transform->m_localPosition.xy - transform->m_localPosition.xy);

			const float cohesionRange = (movement->m_radius + target.m_movement->m_radius) * m_cohesionScale;

			if (distance < cohesionRange && movement->m_velocity != Vector2D::zero &&
				target.m_movement->m_arriveBehavior.HasTarget() &&
				Magnitude(target.m_movement->m_arriveBehavior.GetTarget() - movement->m_arriveBehavior.GetTarget()) < cohesionRange)
			{
				++actorCount;
				heading += Normalize(target.m_movement->m_velocity);
			}
		}

		if (actorCount == 0)
		{
			return Vector2D::zero;
		}

		const Vector2D averageHeading = heading / actorCount;

		const Vector2D desiredVelocity = averageHeading * movement->m_maxSpeed;

		const Vector2D velocityChange = desiredVelocity - movement->m_velocity;
		return velocityChange * (movement->m_maxAcceleration / movement->m_maxSpeed);
	}
}
