#include "stdafx.h"
#include "CohesionBehavior.h"

#include "ArriveBehavior.h"
#include "Components/MovementComponent.h"
#include "Components/TransformComponent.h"
#include "Steering/SteeringSystem.h"

namespace Navigation
{
	Vector2D CohesionBehavior::GetLinearAcceleration(const TransformComponent* transform,
		const MovementComponent*                                               movement,
		const std::vector<NearbyTarget>&                                       nearbyTargets)
	{
		if (!movement->m_arriveBehavior.HasTarget())
		{
			return Vector2D::zero;
		}
		Vector2D heading = Vector2D::zero;

		uint32_t actorCount = 0;

		for (const NearbyTarget& target : nearbyTargets)
		{
			const float distanceSqr   = Terathon::SquaredMag(target.m_transform->m_localPosition.xy - transform->m_localPosition.xy);
			const float cohesionRange = (movement->m_radius + target.m_movement->m_radius) * m_cohesionScale;

			if (distanceSqr < cohesionRange * cohesionRange && movement->m_velocity != Vector2D::zero && target.m_movement->m_arriveBehavior.
				HasTarget() &&
				SquaredMag(movement->m_arriveBehavior.GetTarget() - movement->m_arriveBehavior.GetTarget())
				< cohesionRange * cohesionRange)
			{
				++actorCount;
				heading += target.m_transform->m_localPosition.xy;
			}
		}

		if (actorCount == 0 || heading == Vector2D::zero)
		{
			return Vector2D::zero;
		}

		const Vector2D target = heading / actorCount;

		const Vector2D direction = target - transform->m_localPosition.xy;

		const Vector2D targetVelocity = Normalize(direction) * movement->m_maxSpeed;
		return (targetVelocity - movement->m_velocity) * (movement->m_maxAcceleration / movement->m_maxSpeed);
	}
}
