#include "stdafx.h"
#include "ArriveBehavior.h"

#include "Components/MovementComponent.h"
#include "Components/TransformComponent.h"

#include "Pathfinding/PathFinding.h"

#include "Steering/SteeringSystem.h"


namespace Navigation
{
	void ArriveBehavior::OnArrive(const Point3D target, const TransformComponent* transform, const MovementComponent* movement)
	{
		m_target.reset();
	}


	Vector2D ArriveBehavior::GetLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement,
												   const std::vector<NearbyTarget>& nearbyTargets)
	{
		if (!m_target.has_value())
		{
			return Vector2D::zero;
		}

		if (m_calculatePath)
		{
			m_path = PathFinding::FindPath(transform->m_localPosition, m_target.value());
			m_calculatePath = false;
		}

		const Vector2D direction = m_target.value().xy - transform->m_localPosition.xy;
		const float distance     = SquaredMag(direction);

		if (distance < m_targetRadius * m_targetRadius)
		{
			m_hasArrived = true;
			return Vector2D::zero;
		}

		float targetSpeed = movement->m_maxSpeed;
		if (distance <= m_slowdownRadius * m_slowdownRadius)
		{
			targetSpeed *= distance / (m_slowdownRadius * m_slowdownRadius);
		}

		const Vector2D targetVelocity = Normalize(direction) * targetSpeed;

		return (targetVelocity - movement->m_velocity) * (movement->m_maxAcceleration / movement->m_maxSpeed);
	}

	void ArriveBehavior::SetTarget(const Point3D& point)
	{
		m_target        = point;
		m_hasArrived    = false;
		m_calculatePath = true;
	}
}
