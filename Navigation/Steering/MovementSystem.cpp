#include "stdafx.h"
#include "MovementSystem.h"

#include "Components/MovementComponent.h"
#include "Components/TransformComponent.h"
#include "Managers/TimeManager.h"
#include "Pathfinding/PathFinding.h"
#include "Terrain/Terrain.h"

namespace Navigation
{
	void MovementSystem::Update(uint64_t entityId, TransformComponent* transform, MovementComponent* movement)
	{
		if (movement->m_velocity != Vector2D::zero)
		{
			constexpr float deltaTime = TimeManager::GetTimeStep();

			transform->m_localPosition += Vector3D(movement->m_velocity, 0.0f) * deltaTime;

			transform->m_localPosition.z = PathFinding::m_terrain->GetHeightAtLocation(
				(Point2D) transform->m_localPosition.xy);

			const Vector3D oldDirection = transform->m_localRotation.GetDirectionY();
			const float oldOrientation  = std::atan2(-oldDirection.x, oldDirection.y);

			const Vector2D velocityDir    = Normalize(movement->m_velocity);
			const float targetOrientation = std::atan2(-velocityDir.x, velocityDir.y);

			const float rotation     = RemapAngleRad(targetOrientation - oldOrientation);
			const float rotationSize = abs(rotation);

			if (rotationSize != 0.0f)
			{
				const float diff = abs(Min(movement->m_maxRotation * deltaTime, rotationSize)) * rotation / rotationSize;

				const float newOrientation = oldOrientation + diff;

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
