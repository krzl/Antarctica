#include "stdafx.h"
#include "SeparationBehavior.h"

#include "ArriveBehavior.h"
#include "Entities/MovementComponent.h"
#include "GameObjects/GameObject.h"

namespace Navigation
{
	Vector2D SeparationBehavior::GetLinearAcceleration()
	{
		Vector2D totalAcceleration = Vector2D::zero;

		const float characterRadius = m_movement->GetRadius();

		uint32_t actorCount = 0;

		for (const MovementComponent* target : GetCachedTargets())
		{
			const Vector2D direction = m_movement->GetOwner()->GetPosition().xy - target->GetOwner()->GetPosition().xy;
			const float    distance  = Magnitude(direction);

			const float otherRadius = target->GetRadius();

			const float radiusSum        = characterRadius + otherRadius;

			if (m_movement->HasTarget() && !target->HasTarget())
			{
				continue;
			}

			if (distance < max(radiusSum, radiusSum) && distance > 0.0f)
			{
				const float relativeDistance = distance / radiusSum;
				const float strength = Min(m_decayCoefficient / (relativeDistance * relativeDistance), 1.0f);
				
				totalAcceleration += direction * strength;

				++actorCount;
			}
		}

		if (actorCount == 0 || totalAcceleration == Vector2D::zero)
		{
			return Vector2D::zero;
		}

		return totalAcceleration / actorCount * m_movement->GetMaxAcceleration();
	}
}
