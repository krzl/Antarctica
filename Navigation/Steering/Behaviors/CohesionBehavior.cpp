#include "stdafx.h"
#include "CohesionBehavior.h"

#include "ArriveBehavior.h"
#include "Components/MovementComponent.h"
#include "Components/TransformComponent.h"

namespace Navigation
{
	void CohesionBehavior::InitializeTotalAccelerationCalculation(const TransformComponent* transform, MovementComponent* movement)
	{
		m_heading    = Vector2D::zero;
		m_actorCount = 0;
	}

	void CohesionBehavior::UpdateNearbyEntity(const TransformComponent* transform, MovementComponent* movement,
											  const TransformComponent* nearbyTransform, MovementComponent* nearbyMovement)
	{
		const float distanceSqr   = SquaredMag(nearbyTransform->m_localPosition.xy - transform->m_localPosition.xy);
		const float cohesionRange = (movement->m_colliderRadius + nearbyMovement->m_colliderRadius) * m_cohesionScale;

		if (distanceSqr < cohesionRange * cohesionRange &&
			movement->m_velocity != Vector2D::zero &&
			nearbyMovement->m_arriveBehavior.HasTarget() &&
			movement->m_arriveBehavior.HasTarget() &&
			SquaredMag(movement->m_arriveBehavior.GetTarget() - movement->m_arriveBehavior.GetTarget()) < cohesionRange * cohesionRange)
		{
			++m_actorCount;
			m_heading += nearbyTransform->m_localPosition.xy;
		}
	}

	Vector2D CohesionBehavior::GetFinalLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement)
	{
		PERF_COUNTER(GetFinalLinearAcceleration__CohesionBehavior);
		if (!movement->m_arriveBehavior.HasTarget() || m_actorCount == 0 || m_heading == Vector2D::zero)
		{
			return Vector2D::zero;
		}

		const Vector2D target         = m_heading / m_actorCount;
		const Vector2D direction      = target - transform->m_localPosition.xy;
		const Vector2D targetVelocity = Normalize(direction) * movement->m_maxSpeed;
		return (targetVelocity - movement->m_velocity) * (movement->m_maxAcceleration / movement->m_maxSpeed);
	}
}
