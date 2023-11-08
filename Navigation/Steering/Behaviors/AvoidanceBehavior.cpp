#include "stdafx.h"
#include "AvoidanceBehavior.h"

#include "Components/MovementComponent.h"
#include "Components/TransformComponent.h"
#include "Pathfinding/NavMesh.h"
#include "Pathfinding/PathFinding.h"

namespace Navigation
{
	void AvoidanceBehavior::InitializeTotalAccelerationCalculation(const TransformComponent* transform, MovementComponent* movement)
	{
		if (!movement->m_arriveBehavior.HasTarget())
		{
			return;
		}

		m_closestTransform   = nullptr;
		m_closestDistanceSqr = Min(SquaredMag(movement->m_velocity), 1.0f);
	}

	void AvoidanceBehavior::UpdateNearbyEntity(const TransformComponent* transform, MovementComponent* movement,
											   const TransformComponent* nearbyTransform, MovementComponent* nearbyMovement)
	{
		if (!movement->m_arriveBehavior.HasTarget() ||
			!nearbyMovement->m_arriveBehavior.HasTarget() ||
			SquaredMag(movement->m_arriveBehavior.GetTarget().xy - nearbyMovement->m_arriveBehavior.GetTarget().xy) < 5.0f * 5.0f ||
			Dot(movement->m_velocity, nearbyMovement->m_velocity) > 0.0f)
		{
			return;
		}

		const float distanceSqr = SquaredMag(transform->m_localPosition - nearbyTransform->m_localPosition);

		if (distanceSqr < m_closestDistanceSqr)
		{
			m_closestTransform   = nearbyTransform;
			m_closestDistanceSqr = distanceSqr;
		}
	}

	Vector2D AvoidanceBehavior::GetFinalLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement)
	{
		PERF_COUNTER(GetFinalLinearAcceleration__AvoidanceBehavior);
		if (!movement->m_arriveBehavior.HasTarget())
		{
			return Vector2D::zero;
		}

		Point2D collisionPoint;
		Point2D collisionNormal;
		float penetration;

		{
			bool hasCollided;

			{
				PERF_COUNTER(AvoidanceBehavior__NavCollision);
				hasCollided = PathFinding::m_navMesh->FindCollisionPoint(Sphere{ transform->m_localPosition, Terathon::Sqrt(m_closestDistanceSqr) },
					movement->m_velocity, collisionPoint, collisionNormal, penetration);
			}

			if (hasCollided)
			{
				PERF_COUNTER(AvoidanceBehavior__PostNavCollision);
				const Vector2D awayVector = collisionPoint - transform->m_localPosition.xy;

				const float dot        = movement->m_velocity.x * -awayVector.y + movement->m_velocity.y * awayVector.x;
				const bool avoidToLeft = dot > 0.0f;

				const Vector2D desiredDirection = Normalize(avoidToLeft ? Vector2D(-awayVector.y, awayVector.x) : Vector2D(awayVector.y, -awayVector.x));
				const Vector2D desiredVelocity  = desiredDirection * movement->m_radius * movement->m_maxSpeed;
				const Vector2D force            = desiredVelocity - movement->m_velocity;
				return force * movement->m_maxAcceleration / movement->m_maxSpeed;
			}
		}

		if (m_closestTransform == nullptr)
		{
			return Vector2D::zero;
		}

		const Vector2D awayVector = transform->m_localPosition.xy - m_closestTransform->m_localPosition.xy;

		const float dot        = movement->m_velocity.x * -awayVector.y + movement->m_velocity.y * awayVector.x;
		const bool avoidToLeft = dot > 0.0f;

		const Vector2D desiredDirection = Normalize(avoidToLeft ? Vector2D(-awayVector.y, awayVector.x) : Vector2D(awayVector.y, -awayVector.x));
		const Vector2D desiredVelocity  = desiredDirection * movement->m_radius * movement->m_maxSpeed;
		const Vector2D force            = desiredVelocity - movement->m_velocity;
		return force * movement->m_maxAcceleration / movement->m_maxSpeed;
	}
}
