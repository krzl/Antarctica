#include "stdafx.h"
#include "ArriveBehavior.h"

#include "Entities/MovementComponent.h"
#include "GameObjects/GameObject.h"

namespace Navigation
{
	void ArriveBehavior::OnArrive()
	{
		const Point3D target = m_target.value();

		m_target.reset();
		m_movement->m_onArrive.Dispatch();

		if (m_notifyRadius == 0.0f)
		{
			return;
		}

		for (const MovementComponent* component : GetCachedTargets())
		{
			const Point3D pos     = component->GetOwner()->GetPosition();
			const float   diffSqr = SquaredMag(pos - m_movement->GetOwner()->GetPosition());

			const std::unique_ptr<ArriveBehavior>& otherArrive = component->m_arriveBehavior;
			std::optional<Point3D>&                otherTarget = otherArrive->m_target;
			if (otherTarget.has_value() &&
				SquaredMag(target - otherTarget.value()) < m_notifyRadius * m_notifyRadius &&
				diffSqr < (m_notifyRadius * m_notifyRadius) &&
				SquaredMag(otherTarget.value() - pos) < m_targetOuterRadius * m_targetOuterRadius)
			{
				otherArrive->OnArrive();
			}
		}
	}

	Vector2D ArriveBehavior::GetLinearAcceleration()
	{
		if (!m_target.has_value())
		{
			return Vector2D::zero;
		}

		const Vector2D direction = m_target.value().xy - m_movement->GetOwner()->GetPosition().xy;
		const float    distance  = SquaredMag(direction);

		if (distance < m_targetRadius * m_targetRadius)
		{
			OnArrive();
			return Vector2D::zero;
		}

		float targetSpeed = m_movement->GetMaxSpeed();
		if (distance <= m_slowdownRadius * m_slowdownRadius)
		{
			targetSpeed *= distance / (m_slowdownRadius * m_slowdownRadius);
		}

		const Vector2D targetVelocity = Normalize(direction) * targetSpeed;

		return (targetVelocity - m_movement->GetVelocity()) * (m_movement->GetMaxAcceleration() / m_movement->GetMaxSpeed());
	}
}
