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
		m_iterationCount  = 1;
		m_isMultiThreaded = false;
	}

	void ApplyImpulseSystem::Update(Entity* entity, TransformComponent* transform, Navigation::MovementComponent* movement,
									PhysicsBodyComponent* physicsBody)
	{
		for (const CollisionData& collision : physicsBody->m_collisions)
		{
			Navigation::MovementComponent* otherMovement = nullptr;
			const TransformComponent* otherTransform     = nullptr;
			const PhysicsBodyComponent* otherPhysicsBody = nullptr;

			if (collision.m_entityB)
			{
				const ComponentAccessor& componentAccessor = collision.m_entityB->GetComponentAccessor();
				otherMovement                              = componentAccessor.GetComponent<Navigation::MovementComponent>();
				otherPhysicsBody                           = componentAccessor.GetComponent<PhysicsBodyComponent>();
				otherTransform                             = componentAccessor.GetComponent<TransformComponent>();
			}

			const float inverseMassSum = physicsBody->GetInverseMass() + (otherPhysicsBody ? otherPhysicsBody->GetInverseMass() : 0.0f);

			if (m_currentIteration == 0)
			{
				constexpr float k_slop          = 0.05f; // Penetration allowance
				constexpr float penetrationCorr = 0.4f;  // Penetration correction percentage

				const Vector2D correction = Max(collision.m_penetration - k_slop, 0.0f) * penetrationCorr / inverseMassSum * collision.m_normal;

				movement->m_positionCorrection -= correction * physicsBody->GetInverseMass();

				if (otherMovement)
				{
					otherMovement->m_positionCorrection += correction * otherPhysicsBody->GetInverseMass();
				}
			}

			Vector2D relativeVelocity = -movement->m_velocity;
			if (otherMovement)
			{
				relativeVelocity += otherMovement->m_velocity;
			}

			if (relativeVelocity == Vector2D::zero)
			{
				continue;
			}

			const float contactVelocity = Dot(relativeVelocity, collision.m_normal);
			if (contactVelocity >= 0.0f)
			{
				// entities are separating, ignore resolve
				continue;
			}

			const float j = -(1.0f + collision.m_restitution) * contactVelocity / inverseMassSum;

			float velocityRatio = 0.5f;
			if (otherMovement)
			{
				if (movement->m_arriveBehavior.HasTarget() && otherMovement->m_arriveBehavior.HasTarget())
				{
					const float deltaTransform = Magnitude(movement->m_lastPosition - transform->m_localPosition.xy);
					const float otherDeltaTransform = Magnitude(otherMovement->m_lastPosition - otherTransform->m_localPosition.xy);
					velocityRatio = deltaTransform / (deltaTransform + otherDeltaTransform);
				}
				else
				{
					const float deltaVelocity = Magnitude(movement->m_velocity);
					const float otherDeltaVelocity = Magnitude(otherMovement->m_velocity);
					velocityRatio = otherDeltaVelocity / (deltaVelocity + otherDeltaVelocity);
				}
			}

			const Vector2D impulse = collision.m_normal * j;
			movement->m_velocity -= physicsBody->GetInverseMass() * impulse * Max(0.0f, Min(velocityRatio * 2.0f, 1.0f));

			relativeVelocity = -movement->m_velocity;
			if (otherMovement)
			{
				otherMovement->m_velocity += otherPhysicsBody->GetInverseMass() * impulse * Max(0.0f, Min((1.0f - velocityRatio) * 2.0f, 1.0f));
				relativeVelocity += otherMovement->m_velocity;
			}

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

			movement->m_velocity -= physicsBody->GetInverseMass() * tangentImpulse * Max(0.0f, Min(velocityRatio * 2.0f, 1.0f));

			if (otherMovement)
			{
				otherMovement->m_velocity += otherPhysicsBody->GetInverseMass() * tangentImpulse *
					Max(0.0f, Min((1.0f - velocityRatio) * 2.0f, 1.0f));
			}
		}
	}
}
