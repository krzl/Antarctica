#include "stdafx.h"
#include "SeparationBehavior.h"

#include "ArriveBehavior.h"
#include "Components/MovementComponent.h"
#include "Components/TransformComponent.h"

namespace Navigation
{
	void SeparationBehavior::InitializeTotalAccelerationCalculation(const TransformComponent* transform, MovementComponent* movement)
	{
		m_actorCount        = 0;
		m_totalAcceleration = Vector2D::zero;
	}

	void SeparationBehavior::UpdateNearbyEntity(const TransformComponent* transform, MovementComponent* movement,
												const TransformComponent* nearbyTransform, MovementComponent* nearbyMovement)
	{
		const float characterRadius = movement->m_radius;

		const Vector2D direction = transform->m_localPosition.xy - nearbyTransform->m_localPosition.xy;
		const float distance     = Magnitude(direction);

		const float otherRadius = nearbyMovement->m_radius;

		const float radiusSum = characterRadius + otherRadius;

		if (movement->m_arriveBehavior.HasTarget() && !nearbyMovement->m_arriveBehavior.HasTarget())
		{
			return;
		}

		if (distance < radiusSum && distance > 0.0f)
		{
			const float relativeDistance = distance / radiusSum;
			const float strength         = m_decayCoefficient / (relativeDistance * relativeDistance);

			m_totalAcceleration += direction * strength;

			++m_actorCount;
		}
	}

	Vector2D SeparationBehavior::GetFinalLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement)
	{
		if (m_actorCount == 0 || m_totalAcceleration == Vector2D::zero)
		{
			return Vector2D::zero;
		}

		m_totalAcceleration *= movement->m_maxAcceleration / m_actorCount;

		const float acceleration = Magnitude(m_totalAcceleration);
		if (acceleration > movement->m_maxAcceleration)
		{
			m_totalAcceleration *= movement->m_maxAcceleration / acceleration;
		}

		return m_totalAcceleration / m_actorCount * movement->m_maxAcceleration;
	}
}
