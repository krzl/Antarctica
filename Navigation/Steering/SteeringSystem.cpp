#include "stdafx.h"
#include "SteeringSystem.h"

#include "assimp/code/AssetLib/Blender/BlenderScene.h"
#include "Behaviors/AlignmentBehavior.h"
#include "Behaviors/ArriveBehavior.h"
#include "Behaviors/CohesionBehavior.h"
#include "Components/MovementComponent.h"
#include "Components/TransformComponent.h"
#include "Entities/World.h"
#include "Managers/TimeManager.h"

namespace Navigation
{
	void SteeringSystem::OnUpdateStart()
	{
		System::OnUpdateStart();
		m_movementTester.DrawMenu();
	}

	void SteeringSystem::Update(uint64_t entityId, TransformComponent* transform, MovementComponent* movement)
	{
		if (movement->m_enableMovementTester)
		{
			m_movementTester.UpdateComponent(movement, transform);
		}

		const float maxRadiusToCheck = Max(movement->m_radius * movement->m_cohesionBehavior.GetCohesionScale(),
			movement->m_radius * movement->m_alignmentBehavior.GetCohesionScale());

		std::vector<NearbyTarget> targets;
		targets.reserve(25);

		const std::vector<Entity*> nearbyEntities = World::Get()->GetQuadtree().FindNearby(Sphere{ transform->m_localPosition, maxRadiusToCheck });

		for (Entity* entity : nearbyEntities)
		{
			ComponentAccessor componentAccessor = entity->GetComponentAccessor();

			const TransformComponent* nearbyTransform = componentAccessor.GetComponent<TransformComponent>();
			const MovementComponent* nearbyMovement   = componentAccessor.GetComponent<MovementComponent>();

			if (nearbyTransform && nearbyMovement && transform != nearbyTransform)
			{
				targets.push_back({ nearbyTransform, nearbyMovement });
			}
		}

		Vector2D acceleration = movement->m_steeringPipeline.GetLinearAcceleration(transform, movement, targets);

		if (!movement->m_arriveBehavior.HasTarget())
		{
			acceleration *= movement->m_decelerationFactor;
		}

		const float deltaTime = TimeManager::GetInstance()->GetTimeStep();

		if (acceleration == Vector2D::zero)
		{
			if (SquaredMag(movement->m_velocity) != 0.0f)
			{
				const Vector2D velocityDelta = Normalize(movement->m_velocity) * movement->m_maxAcceleration * deltaTime;
				if (SquaredMag(velocityDelta) > SquaredMag(movement->m_velocity))
				{
					movement->m_velocity = Vector2D::zero;
				}
				else
				{
					//movement->m_velocity -= velocityDelta; //TODO: check if can be removed
					movement->m_velocity = Vector2D::zero;
				}
			}
		}
		else
		{
			movement->m_velocity += acceleration * deltaTime;
		}
	}
}
