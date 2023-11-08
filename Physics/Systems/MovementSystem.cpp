#include "stdafx.h"
#include "MovementSystem.h"

#include "Archetypes/ComponentAccessor.h"
#include "Components/MovementComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"

#include "Debug/DebugDrawManager.h"

#include "Entities/World.h"
#include "Managers/TimeManager.h"
#include "Pathfinding/PathFinding.h"
#include "Terrain/Terrain.h"

namespace Physics
{
	void MovementSystem::Update(Entity* entity, TransformComponent* transform, Navigation::MovementComponent* movement)
	{
		if (movement->m_force != Vector2D::zero || movement->m_velocity != Vector2D::zero || movement->m_positionCorrection != Vector2D::zero)
		{
			constexpr float deltaTime = TimeManager::GetTimeStep();

			Point2D targetPosition    = (Point2D) transform->m_localPosition.xy + movement->m_velocity * deltaTime + movement->m_positionCorrection;
			Point2D oldTargetPosition = targetPosition;

			const ComponentAccessor& componentAccessor = entity->GetComponentAccessor();
			if (const PhysicsBodyComponent* physicsBody = componentAccessor.GetComponent<PhysicsBodyComponent>())
			{
				Navigation::NavMesh* navMesh = Navigation::PathFinding::m_navMesh;

				Point2D collisionPoint;
				Vector2D collisionNormal;

				if (targetPosition != transform->m_localPosition.xy &&
					navMesh->FindCollisionPoint((Point2D) transform->m_localPosition.xy, targetPosition, collisionPoint, collisionNormal))
				{
					const Point2D deltaPosition   = targetPosition - transform->m_localPosition.xy;
					const Vector2D slideDirection = deltaPosition - Dot(-collisionNormal, deltaPosition) * (-collisionNormal);
					const Point2D slideStart      = (Point2D) transform->m_localPosition.xy;
					const Point2D slideEnd        = slideStart + movement->m_positionCorrection + slideDirection * deltaTime;

					movement->m_velocity = movement->m_velocity - Dot(-collisionNormal, movement->m_velocity) * (-collisionNormal);

					if (navMesh->FindCollisionPoint(slideStart, slideEnd, collisionPoint, collisionNormal))
					{
						targetPosition = collisionPoint + Normalize(collisionNormal) * 0.00001f;
					}
					else
					{
						targetPosition = slideEnd;
					}
				}

				if (!navMesh->m_triangles[navMesh->FindTriangleId(targetPosition)].m_isNavigable)
				{
					//FALLBACK
					targetPosition = transform->m_localPosition.xy;
				}

				movement->m_velocity += movement->m_force * physicsBody->GetInverseMass() * deltaTime / 2.0f;

				if (movement->m_velocity != Vector2D::zero)
				{
					if (SquaredMag(movement->m_velocity) > movement->m_maxSpeed * movement->m_maxSpeed)
					{
						movement->m_velocity = Normalize(movement->m_velocity) * movement->m_maxSpeed;
					}
				}
			}

			movement->m_lastPosition = (Point2D) transform->m_localPosition.xy;

			transform->m_localPosition.xy = targetPosition;
			transform->m_localPosition.z  = Navigation::PathFinding::m_terrain->GetHeightAtLocation((Point2D) transform->m_localPosition.xy);

			movement->m_force              = Vector2D::zero;
			movement->m_positionCorrection = Vector2D::zero;


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
