#include "stdafx.h"
#include "CollisionGatherSystem.h"

#include "Components/MovementComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"
#include "Entities/World.h"
#include "Managers/TimeManager.h"

#include "Pathfinding/NavMesh.h"
#include "Pathfinding/PathFinding.h"

#include "PhysicsBody/CollisionData.h"

namespace Physics
{
	void CollisionGatherSystem::Update(Entity* entity, TransformComponent* transform, Navigation::MovementComponent* movement,
									   PhysicsBodyComponent* physicsBody)
	{
		movement->m_velocity += movement->m_force * physicsBody->GetInverseMass() * TimeManager::GetInstance()->GetTimeStep() / 2.0f;

		physicsBody->m_collisions.resize(0);
		World::Get()->GetQuadtree().FindNearby(Sphere{ transform->m_localPosition, movement->m_colliderRadius },
			[=](Entity* other)
			{
				if (other->GetInstanceId() < entity->GetInstanceId()) // avoid duplicate collisions and collision with self
				{
					const ComponentAccessor& componentAccessor = other->GetComponentAccessor();

					const TransformComponent* otherTransform;
					const Navigation::MovementComponent* otherMovement;
					const PhysicsBodyComponent* otherPhysicsBody;

					if (((otherTransform   = componentAccessor.GetComponent<TransformComponent>())) &&
						((otherMovement    = componentAccessor.GetComponent<Navigation::MovementComponent>())) &&
						((otherPhysicsBody = componentAccessor.GetComponent<PhysicsBodyComponent>())))
					{
						Vector2D normal         = otherTransform->m_localPosition.xy - transform->m_localPosition.xy;
						const float distanceSqr = SquaredMag(normal);
						const float radius      = movement->m_colliderRadius + otherMovement->m_colliderRadius;

						if (distanceSqr >= radius * radius)
						{
							return;
						}

						if (distanceSqr == 0.0f)
						{
							physicsBody->m_collisions.emplace_back(CollisionData{
								entity,
								other,
								Min(physicsBody->m_restitution, otherPhysicsBody->m_restitution),
								Terathon::Sqrt(physicsBody->m_staticFriction * otherPhysicsBody->m_staticFriction),
								Terathon::Sqrt(physicsBody->m_dynamicFriction * otherPhysicsBody->m_dynamicFriction),
								movement->m_colliderRadius,
								Vector2D(1.0f, 0.0f),
								(Point2D) transform->m_localPosition.xy
							});
						}
						else
						{
							const float distance = Terathon::Sqrt(distanceSqr);

							normal = normal / distance; // normalization

							physicsBody->m_collisions.emplace_back(CollisionData{
								entity,
								other,
								Min(physicsBody->m_restitution, otherPhysicsBody->m_restitution),
								Terathon::Sqrt(physicsBody->m_staticFriction * otherPhysicsBody->m_staticFriction),
								Terathon::Sqrt(physicsBody->m_dynamicFriction * otherPhysicsBody->m_dynamicFriction),
								movement->m_colliderRadius - distance,
								normal,
								normal * movement->m_colliderRadius + (Point2D) transform->m_localPosition.xy
							});
						}
					}
				}
			});

		Navigation::NavMesh* navMesh = Navigation::PathFinding::m_navMesh;

		Vector2D collisionNormal;
		Point2D collisionPoint;
		float penetration;

		if (navMesh->FindCollisionPoint(Sphere{ transform->m_localPosition, movement->m_colliderRadius }, collisionPoint, collisionNormal, penetration))
		{
			const Vector2D normal = transform->m_localPosition.xy == collisionPoint ?
										collisionNormal :
										-Normalize(transform->m_localPosition.xy - collisionPoint);

			physicsBody->m_collisions.emplace_back(CollisionData{
				entity,
				nullptr,
				physicsBody->m_restitution,
				physicsBody->m_staticFriction,
				physicsBody->m_dynamicFriction,
				penetration,
				normal,
				collisionPoint
			});
		}
	}
}
