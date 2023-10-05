#include "stdafx.h"
#include "AlignmentBehavior.h"

#include "ArriveBehavior.h"
#include "Components/MovementComponent.h"
#include "Components/TransformComponent.h"

namespace Navigation
{
	void AlignmentBehavior::InitializeTotalAccelerationCalculation(const TransformComponent* transform, MovementComponent* movement)
	{
		m_heading    = Vector2D::zero;
		m_actorCount = 0;
	}

	void AlignmentBehavior::UpdateNearbyEntity(const TransformComponent* transform, MovementComponent* movement,
											   const TransformComponent* nearbyTransform, MovementComponent* nearbyMovement)
	{
		if (!movement->m_arriveBehavior.HasTarget())
		{
			return;
		}

		const float distance      = SquaredMag(nearbyTransform->m_localPosition.xy - transform->m_localPosition.xy);
		const float cohesionRange = (movement->m_radius + nearbyMovement->m_radius) * m_cohesionScale;

		if (distance < cohesionRange && movement->m_velocity != Vector2D::zero &&
			nearbyMovement->m_arriveBehavior.HasTarget() &&
			SquaredMag(nearbyMovement->m_arriveBehavior.GetTarget() - movement->m_arriveBehavior.GetTarget()) < cohesionRange * cohesionRange)
		{
			++m_actorCount;
			m_heading += Normalize(nearbyMovement->m_velocity);
		}
	}

	Vector2D AlignmentBehavior::GetFinalLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement)
	{
		if (!movement->m_arriveBehavior.HasTarget() || m_actorCount == 0)
		{
			return Vector2D::zero;
		}

		const Vector2D averageHeading  = m_heading / m_actorCount;
		const Vector2D desiredVelocity = averageHeading * movement->m_maxSpeed;
		const Vector2D force  = desiredVelocity - movement->m_velocity;
		return force * movement->m_maxAcceleration / movement->m_maxSpeed;
	}
}
