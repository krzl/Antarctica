#include "stdafx.h"
#include "ApplyImpulseSystem.h"

#include "Components/MovementComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"
#include "Entities/World.h"
#include "PhysicsBody/CollisionData.h"

namespace Physics
{
	ApplyImpulseSystem::ApplyImpulseSystem()
	{
		m_iterationCount  = 10;
		m_isMultiThreaded = false; //TODO: CHECK THAT
	}

	void ApplyImpulseSystem::Update(uint64_t entityId, TransformComponent* transform, Navigation::MovementComponent* movement,
									PhysicsBodyComponent* physicsBody)
	{
		for (const CollisionData& collision : physicsBody->m_collisions)
		{
			const ComponentAccessor& componentAccessor   = collision.m_entityB->GetComponentAccessor();
			const TransformComponent* otherTransform     = componentAccessor.GetComponent<TransformComponent>();
			Navigation::MovementComponent* otherMovement = componentAccessor.GetComponent<Navigation::MovementComponent>();
			const PhysicsBodyComponent* otherPhysicsBody = componentAccessor.GetComponent<PhysicsBodyComponent>();

			const Vector2D ra = collision.m_contactPoint - transform->m_localPosition.xy;
			const Vector2D rb = collision.m_contactPoint - otherTransform->m_localPosition.xy;

			Vector2D relativeVelocity = otherMovement->m_velocity - movement->m_velocity;

			if (relativeVelocity == Vector2D::zero)
			{
				continue;
			}
			
			const float contactVelocity = Dot(relativeVelocity, collision.m_normal);


			if (contactVelocity > 0.0f)
			{
				// entities are separating, ignore resolve
				continue;
			}
			const float raCrossN       = ra.x * collision.m_normal.y - ra.y * collision.m_normal.x;
			const float rBCrossN       = rb.x * collision.m_normal.y - rb.y * collision.m_normal.x;
			const float inverseMassSum = physicsBody->GetInverseMass() + otherPhysicsBody->GetInverseMass();

			const float j = -(1.0f + collision.m_restitution) * contactVelocity / inverseMassSum;

			const Vector2D impulse = collision.m_normal * j;
			movement->m_velocity -= physicsBody->GetInverseMass() * impulse;
			otherMovement->m_velocity += otherPhysicsBody->GetInverseMass() * impulse;

			relativeVelocity = otherMovement->m_velocity - movement->m_velocity;
			const Vector2D t = Normalize(relativeVelocity - (collision.m_normal * Dot(relativeVelocity, collision.m_normal)));

			if (relativeVelocity == Vector2D::zero || IsNaN(t))
			{
				continue;
			}

			const float jt = -Dot(relativeVelocity, t) / inverseMassSum;
			if (jt == 0.0f)
			{
				continue;
			}

			Vector2D tangentImpulse;
			if (std::abs(jt) < j * collision.m_staticFriction)
			{
				tangentImpulse = t * jt;
			}
			else
			{
				tangentImpulse = t * -j * collision.m_dynamicFriction;
			}

			movement->m_velocity -= physicsBody->GetInverseMass() * tangentImpulse;
			otherMovement->m_velocity += otherPhysicsBody->GetInverseMass() * tangentImpulse;
		}
	}
}
