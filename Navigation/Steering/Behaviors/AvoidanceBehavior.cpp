#include "stdafx.h"
#include "AvoidanceBehavior.h"

#include "Components/MovementComponent.h"
#include "Components/TransformComponent.h"

#include "Debug/DebugDrawManager.h"

#include "Pathfinding/NavMesh.h"
#include "Pathfinding/PathFinding.h"

namespace Navigation
{
	void AvoidanceBehavior::InitializeTotalAccelerationCalculation(const TransformComponent* transform, MovementComponent* movement)
	{
		if (movement->m_velocity == Vector2D::zero)
		{
			return;
		}

		m_closestMovement    = nullptr;
		m_closestTransform   = nullptr;
		m_closestDistanceSqr = SquaredMag(movement->m_velocity);;
	}

	void AvoidanceBehavior::UpdateNearbyEntity(const TransformComponent* transform, MovementComponent* movement,
											   const TransformComponent* nearbyTransform, MovementComponent* nearbyMovement)
	{
		if (movement->m_velocity == Vector2D::zero ||
			nearbyMovement->m_velocity == Vector2D::zero ||
			Dot(movement->m_velocity, nearbyMovement->m_velocity) > 0.0f)
		{
			return;
		}

		const float distanceSqr = SquaredMag(transform->m_localPosition - nearbyTransform->m_localPosition);

		if (distanceSqr < m_closestDistanceSqr)
		{
			m_closestTransform   = nearbyTransform;
			m_closestMovement    = nearbyMovement;
			m_closestDistanceSqr = distanceSqr;
		}
	}

	Vector2D AvoidanceBehavior::GetFinalLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement)
	{
		if (movement->m_velocity == Vector2D::zero)
		{
			return Vector2D::zero;
		}

		Point2D collisionPoint;
		Point2D collisionNormal;
		float penetration;

		if (PathFinding::m_navMesh->FindCollisionPoint(transform->m_localPosition, movement->m_velocity, Terathon::Sqrt(m_closestDistanceSqr),
			collisionPoint, collisionNormal, penetration))
		{
			const Vector2D awayVector = collisionPoint - transform->m_localPosition.xy;

			const float dot        = movement->m_velocity.x * -awayVector.y + movement->m_velocity.y * awayVector.x;
			const bool avoidToLeft = dot > 0.0f;

			const Vector2D desiredDirection = Normalize(avoidToLeft ? Vector2D(-awayVector.y, awayVector.x) : Vector2D(awayVector.y, -awayVector.x));
			const Vector2D desiredVelocity  = desiredDirection * movement->m_radius * movement->m_maxSpeed;
			const Vector2D force            = desiredVelocity - movement->m_velocity;
			return force * movement->m_maxAcceleration / movement->m_maxSpeed;
		}

		return Vector2D::zero;
	}
}
