﻿#include "stdafx.h"
#include "ArriveBehavior.h"

#include "Components/MovementComponent.h"
#include "Components/TransformComponent.h"

#include "Pathfinding/NavMesh.h"
#include "Pathfinding/PathFinding.h"


namespace Navigation
{
	void ArriveBehavior::OnArrive(const Point3D target, const TransformComponent* transform, const MovementComponent* movement)
	{
		m_target.reset();
		m_path.reset();
	}

	void ArriveBehavior::InitializeTotalAccelerationCalculation(const TransformComponent* transform, MovementComponent* movement) { }

	void ArriveBehavior::UpdateNearbyEntity(const TransformComponent* transform, MovementComponent* movement,
											const TransformComponent* nearbyTransform, MovementComponent* nearbyMovement) { }


	Vector2D ArriveBehavior::GetFinalLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement)
	{
		if (!m_target.has_value())
		{
			return Vector2D::zero;
		}

		if (m_framesUntilCalculatePath == 0)
		{
			m_path = PathFinding::FindPath(transform->m_localPosition, m_target.value());

			m_framesUntilCalculatePath = -1;

			if (!m_path.has_value())
			{
				m_hasArrived = true;
				m_target.reset();
			}
			else
			{
				m_currentPathSegment = m_path.value().begin();
			}
		}
		else if (m_framesUntilCalculatePath > 0)
		{
			--m_framesUntilCalculatePath;
		}

		if (!m_path.has_value())
		{
			return Vector2D::zero;
		}

		if (m_currentPathSegment != m_path.value().end())
		{
			auto nextSegment = m_currentPathSegment;
			++nextSegment;

			if (nextSegment == m_path.value().end())
			{
				if (PathFinding::m_navMesh->DoesDirectPathExists(transform->m_localPosition, m_target.value()))
				{
					++m_currentPathSegment;
				}
			}
			else
			{
				if (PathFinding::m_navMesh->DoesDirectPathExists(*nextSegment, transform->m_localPosition))
				{
					++m_currentPathSegment;
				}
			}
		}
		else
		{
			const Vector2D directionToEnd = m_target.value().xy - transform->m_localPosition.xy;
			const float distanceToEnd     = SquaredMag(directionToEnd);

			if (distanceToEnd < m_targetRadius * m_targetRadius)
			{
				m_hasArrived = true;
				return Vector2D::zero;
			}
		}

		const Point3D nextTarget = m_currentPathSegment == m_path.value().end() ?
									   m_target.value() :
									   PathFinding::m_navMesh->GetVertexPosition(*m_currentPathSegment);

		const Vector2D direction = nextTarget.xy - transform->m_localPosition.xy;
		const float distance     = SquaredMag(direction);

		float targetSpeed = movement->m_maxSpeed;
		if (distance <= m_slowdownRadius * m_slowdownRadius)
		{
			targetSpeed *= distance / (m_slowdownRadius * m_slowdownRadius);
		}

		const Vector2D targetVelocity = Normalize(direction) * targetSpeed;

		return (targetVelocity - movement->m_velocity) * (movement->m_maxAcceleration / movement->m_maxSpeed);
	}

	void ArriveBehavior::SetTarget(const Point3D& point, const uint32_t delay)
	{
		m_target                   = point;
		m_hasArrived               = false;
		m_framesUntilCalculatePath = delay;
	}
}
