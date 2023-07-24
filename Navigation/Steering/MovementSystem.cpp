#include "stdafx.h"
#include "MovementSystem.h"

#include "Components/MovementComponent.h"
#include "Components/TransformComponent.h"
#include "Managers/TimeManager.h"

namespace Navigation
{
	void MovementSystem::Update(uint64_t entityId, TransformComponent* transform, MovementComponent* movement)
	{
		if (movement->m_velocity != Vector2D::zero)
		{
			if (SquaredMag(movement->m_velocity) > movement->m_maxSpeed * movement->m_maxSpeed)
			{
				movement->m_velocity = Normalize(movement->m_velocity) * movement->m_maxSpeed;
			}

			transform->m_localPosition += Vector3D(movement->m_velocity, 0.0f) * TimeManager::GetInstance()->GetTimeStep();

			const Vector3D oldDirection   = transform->m_localRotation.GetDirectionY();
			const float    oldOrientation = std::atan2(-oldDirection.x, oldDirection.y);

			const Vector2D velocityDir       = Normalize(movement->m_velocity);
			const float    targetOrientation = std::atan2(-velocityDir.x, velocityDir.y);

			const float rotation     = RemapAngleRad(targetOrientation - oldOrientation);
			const float rotationSize = abs(rotation);

			if (rotationSize != 0.0f)
			{
				const float diff = abs(Min(movement->m_maxRotation * TimeManager::GetInstance()->GetTimeStep(), rotationSize)) * rotation /
					rotationSize;

				const float newOrientation = oldOrientation + diff;

				if (isnan(targetOrientation) || isnan(newOrientation))
				{
					__debugbreak();
				}

				if (abs(AngleDifference(oldOrientation, targetOrientation)) < abs(AngleDifference(oldOrientation, newOrientation)))
				{
					transform->m_localRotation = Quaternion::MakeRotationZ(targetOrientation);
				}
				else
				{
					transform->m_localRotation = Quaternion::MakeRotationZ(newOrientation);
				}
			}
		}
	}
}
