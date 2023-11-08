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
		if (movement->m_arriveBehavior.HasTarget() && !nearbyMovement->m_arriveBehavior.HasTarget())
		{
			return;
		}

		const Vector2D direction = transform->m_localPosition.xy - nearbyTransform->m_localPosition.xy;
		const float distanceSqr  = SquaredMag(direction);

		if (distanceSqr < movement->m_colliderRadius * 4 * movement->m_colliderRadius && distanceSqr > 0.0f)
		{
			const float radiusSum = movement->m_colliderRadius + nearbyMovement->m_colliderRadius;
			const float distance  = Terathon::Sqrt(distanceSqr);
			m_totalAcceleration += direction * (1.0f - ((distance - radiusSum) / (movement->m_colliderRadius * 4 - radiusSum)));
			++m_actorCount;
		}
	}

	Vector2D SeparationBehavior::GetFinalLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement)
	{
		PERF_COUNTER(GetFinalLinearAcceleration__SeparationBehavior);
		if (m_actorCount == 0 || m_totalAcceleration == Vector2D::zero)
		{
			return Vector2D::zero;
		}

		return Normalize(m_totalAcceleration) * movement->m_maxAcceleration;
	}
}
