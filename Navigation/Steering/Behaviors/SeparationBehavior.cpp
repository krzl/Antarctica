#include "stdafx.h"
#include "SeparationBehavior.h"

#include "ArriveBehavior.h"
#include "Components/MovementComponent.h"
#include "Components/TransformComponent.h"
#include "Steering/SteeringSystem.h"

namespace Navigation
{
	Vector2D SeparationBehavior::GetLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement,
													   const std::vector<NearbyTarget>& nearbyTargets)
	{
		Vector2D totalAcceleration = Vector2D::zero;

		const float characterRadius = movement->m_radius;

		uint32_t actorCount = 0;

		for (const NearbyTarget& target : nearbyTargets)
		{
			const Vector2D direction = transform->m_localPosition.xy - target.m_transform->m_localPosition.xy;
			const float distance     = Magnitude(direction);

			const float otherRadius = target.m_movement->m_radius;

			const float radiusSum = characterRadius + otherRadius;

			if (movement->m_arriveBehavior.HasTarget() && !target.m_movement->m_arriveBehavior.HasTarget())
			{
				continue;
			}

			if (distance < radiusSum && distance > 0.0f)
			{
				const float relativeDistance = distance / radiusSum;
				const float strength         = m_decayCoefficient / (relativeDistance * relativeDistance);

				totalAcceleration += direction * strength;

				++actorCount;
			}
		}

		if (actorCount == 0 || totalAcceleration == Vector2D::zero)
		{
			return Vector2D::zero;
		}

		totalAcceleration *= movement->m_maxAcceleration / actorCount;

		const float acceleration = Magnitude(totalAcceleration);
		if (acceleration > movement->m_maxAcceleration)
		{
			totalAcceleration *= movement->m_maxAcceleration / acceleration;
		}

		return totalAcceleration / actorCount * movement->m_maxAcceleration;
	}
}
