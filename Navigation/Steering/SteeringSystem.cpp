#include "stdafx.h"
#include "SteeringSystem.h"

#include "Behaviors/AlignmentBehavior.h"
#include "Behaviors/ArriveBehavior.h"
#include "Behaviors/CohesionBehavior.h"
#include "Components/MovementComponent.h"
#include "Components/TransformComponent.h"
#include "Entities/World.h"

#include "Managers/FrameCounter.h"
#include "Managers/TimeManager.h"

namespace Navigation
{
	void SteeringSystem::DrawMovementTester()
	{
		m_movementTester.DrawMenu();
	}

	void SteeringSystem::OnStepLockStart()
	{
		System::OnStepLockStart();
		m_movementTester.m_hasComponentsToTest = false;
	}

	void SteeringSystem::OnStepLockEnd()
	{
		System::OnStepLockEnd();
		m_movementTester.m_hasValuesChanged = false;
	}

	void SteeringSystem::Update(uint64_t entityId, TransformComponent* transform, MovementComponent* movement)
	{
		if (movement->m_enableMovementTester && m_frameCounter->m_stepLockFramesPending == 0)
		{
			m_movementTester.UpdateComponent(movement, transform);
		}

		const float maxRadiusToCheck = Max(movement->m_radius * movement->m_cohesionBehavior.GetCohesionScale(),
			movement->m_radius * movement->m_alignmentBehavior.GetCohesionScale());

		movement->m_steeringPipeline.InitializeTotalAccelerationCalculation(transform, movement);

		World::Get()->GetQuadtree().FindNearby(Sphere{ transform->m_localPosition, maxRadiusToCheck },
			[entityId, &movement, &transform](Entity* entity)
			{
				const ComponentAccessor& componentAccessor = entity->GetComponentAccessor();

				const TransformComponent* nearbyTransform = componentAccessor.GetComponent<TransformComponent>();
				MovementComponent* nearbyMovement         = componentAccessor.GetComponent<MovementComponent>();

				if (nearbyTransform && nearbyMovement && entity->GetInstanceId() != entityId)
				{
					movement->m_steeringPipeline.UpdateNearbyEntity(transform, movement, nearbyTransform, nearbyMovement);
				}
			});

		const float deltaTime = TimeManager::GetInstance()->GetTimeStep();

		movement->m_force = movement->m_steeringPipeline.GetFinalLinearAcceleration(transform, movement);
		
		if (movement->m_force == Vector2D::zero && movement->m_velocity != Vector2D::zero)
		{
			const Vector2D velocityDelta = Normalize(movement->m_velocity) * movement->m_maxAcceleration * deltaTime;
			if (SquaredMag(velocityDelta) > SquaredMag(movement->m_velocity))
			{
				movement->m_velocity = Vector2D::zero;
			}
			else
			{
				movement->m_velocity -= velocityDelta;
				//movement->m_velocity = Vector2D::zero;
			}
		}
	}
}
