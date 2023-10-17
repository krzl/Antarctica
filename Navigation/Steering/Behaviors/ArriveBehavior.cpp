#include "stdafx.h"
#include "ArriveBehavior.h"

#include "Components/MovementComponent.h"
#include "Components/TransformComponent.h"

#include "Pathfinding/NavMesh.h"
#include "Pathfinding/PathFinding.h"


namespace Navigation
{
	void ArriveBehavior::InitializeTotalAccelerationCalculation(const TransformComponent* transform, MovementComponent* movement) { }

	void ArriveBehavior::UpdateNearbyEntity(const TransformComponent* transform, MovementComponent* movement,
											const TransformComponent* nearbyTransform, MovementComponent* nearbyMovement)
	{
		if (!m_target.has_value() || m_hasArrived)
		{
			return;
		}

		if (nearbyMovement->m_arriveBehavior.m_hasArrived &&
			SquaredMag(nearbyMovement->m_arriveBehavior.GetTarget().xy - GetTarget().xy) < m_targetRadius * m_targetRadius &&
			SquaredMag(transform->m_localPosition.xy - GetTarget().xy) < m_outerTargetRadius * m_outerTargetRadius)
		{
			m_hasArrived = true;
		}
	}


	bool ArriveBehavior::HasArrivedCheck(const TransformComponent* transform)
	{
		const Vector2D directionToEnd = m_target.value().xy - transform->m_localPosition.xy;
		const float distanceToEnd     = SquaredMag(directionToEnd);

		return distanceToEnd < m_targetRadius * m_targetRadius;
	}

	bool ArriveBehavior::RecalculatePath(const TransformComponent* transform)
	{
		m_path = PathFinding::FindPath(transform->m_localPosition, m_target.value());

		if (m_path.has_value())
		{
			m_currentPathSegment = m_path.value().begin();
		}

		return m_path.has_value();
	}

	void ArriveBehavior::AdjustNextPathSegment(const TransformComponent* transform)
	{
		if (m_currentPathSegment == m_path.value().end())
		{
			if (!PathFinding::m_navMesh->DoesDirectPathExists(transform->m_localPosition, m_target.value()))
			{
				if (!RecalculatePath(transform))
				{
					m_framesUntilCalculatePath = 5;
				}
				return;
			}
		}
		else
		{
			if (!PathFinding::m_navMesh->DoesDirectPathExists(*m_currentPathSegment, transform->m_localPosition))
			{
				if (!RecalculatePath(transform))
				{
					m_framesUntilCalculatePath = 5;
				}
				return;
			}
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
	}

	Vector2D ArriveBehavior::GetFinalLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement)
	{
		if (!m_target.has_value() || m_hasArrived)
		{
			return Vector2D::zero;
		}

		if (m_framesUntilCalculatePath == 0)
		{
			m_framesUntilCalculatePath = -1;

			if (!RecalculatePath(transform))
			{
				if (m_retryCalculatePath)
				{
					m_framesUntilCalculatePath = 5;
				}
				else
				{
					m_hasArrived = true;
				}

				return Vector2D::zero;
			}

			m_currentPathSegment = m_path.value().begin();
			m_retryCalculatePath = true;
		}
		else if (m_framesUntilCalculatePath != -1)
		{
			--m_framesUntilCalculatePath;
		}

		if (m_path.has_value())
		{
			AdjustNextPathSegment(transform);

			if (m_framesUntilCalculatePath != -1)
			{
				return Vector2D::zero;
			}
		}

		if (HasArrivedCheck(transform))
		{
			m_hasArrived = true;
			return Vector2D::zero;
		}

		const Point3D nextTarget = !m_path.has_value() || m_currentPathSegment == m_path.value().end() ?
									   m_target.value() :
									   PathFinding::m_navMesh->GetVertexPosition(*m_currentPathSegment);


		const Vector2D direction      = nextTarget.xy - transform->m_localPosition.xy;
		const Vector2D targetVelocity = Normalize(direction) * movement->m_maxSpeed;

		return (targetVelocity - movement->m_velocity) * movement->m_maxAcceleration / movement->m_maxSpeed;
	}

	void ArriveBehavior::SetTarget(const Point3D& point, const uint32_t delay)
	{
		m_target                   = point;
		m_hasArrived               = false;
		m_framesUntilCalculatePath = delay;
		m_retryCalculatePath       = false;
		m_path.reset();
	}
}
