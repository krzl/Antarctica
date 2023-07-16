#include "stdafx.h"
#include "MovementComponent.h"

#include "GameObjects/GameObject.h"
#include "GameObjects/World.h"

#include "Steering/Behaviors/AlignmentBehavior.h"
#include "Steering/Behaviors/ArriveBehavior.h"
#include "Steering/Behaviors/CohesionBehavior.h"
#include "Steering/Behaviors/SeparationBehavior.h"

namespace Navigation
{
	MovementComponent::MovementComponent()
	{
		m_isTickable = true;

		m_arriveBehavior     = CreateBehavior<ArriveBehavior>();
		m_separationBehavior = CreateBehavior<SeparationBehavior>();
		m_cohesionBehavior   = CreateBehavior<CohesionBehavior>();
		m_alignmentBehavior  = CreateBehavior<AlignmentBehavior>();
	}

	void MovementComponent::MoveTo(const Point3D targetPosition) const
	{
		m_arriveBehavior->SetTarget(targetPosition);
	}

	Point3D MovementComponent::GetTarget() const
	{
		return m_arriveBehavior->GetTarget();
	}

	bool MovementComponent::HasTarget() const
	{
		return m_arriveBehavior->HasTarget();
	}

	BoundingBox MovementComponent::GetBoundingBox() const
	{
		const Vector3D extend(GetRadius(), GetRadius(), GetRadius());
		return BoundingBox(GetOwner()->GetPosition() - extend, GetOwner()->GetPosition() + extend);
	}

	void MovementComponent::Tick(const float deltaTime)
	{
		const float maxRadiusToCheck = Max(m_arriveBehavior->GetNotifyRadius(),
										   Max(GetRadius() * m_cohesionBehavior->GetCohesionScale(),
											   (GetRadius() * m_alignmentBehavior->GetCohesionScale())));

		m_targets.clear();
		const std::vector<GameObject*> nearbyObjects = World::Get()->GetQuadtree().FindNearby(Sphere{ GetOwner()->GetPosition(), maxRadiusToCheck });
		
		for (GameObject* gameObject : nearbyObjects)
		{
			if (MovementComponent* movementComponent = gameObject->GetComponent<MovementComponent>())
			{
				m_targets.push_back(movementComponent);
			}
		}
		
		Vector2D acceleration = m_steeringPipeline.GetLinearAcceleration(this);

		if (!HasTarget())
		{
			acceleration *= m_decelerationFactor;
		}

		if (acceleration == Vector2D::zero)
		{
			if (SquaredMag(m_velocity) != 0.0f)
			{
				const Vector2D velocityDelta = Normalize(m_velocity) * m_maxAcceleration * deltaTime;
				if (SquaredMag(velocityDelta) > SquaredMag(m_velocity))
				{
					m_velocity = Vector2D::zero;
				}
				else
				{
					m_velocity -= velocityDelta;
					m_velocity = Vector2D::zero;
				}
			}
		}
		else
		{
			m_velocity += acceleration * deltaTime;
		}

		if (m_velocity != Vector2D::zero)
		{
			if (SquaredMag(m_velocity) > GetMaxSpeed() * GetMaxSpeed())
			{
				m_velocity = Normalize(m_velocity) * GetMaxSpeed();
			}

			GetOwner()->SetPosition(GetOwner()->GetPosition() + Vector3D(m_velocity, 0.0f) * deltaTime);
			
			const Vector3D oldDirection = GetOwner()->GetRotation().GetDirectionY();
			const float    oldOrientation = std::atan2(-oldDirection.x, oldDirection.y);
			
			const Vector2D test              = Normalize(m_velocity);
			const float    targetOrientation = std::atan2(-test.x, test.y);

			const float rotation     = RemapAngleRad(targetOrientation - oldOrientation);
			const float rotationSize = abs(rotation);

			if (rotationSize != 0.0f)
			{
				const float diff = abs(Min(m_maxRotation * deltaTime, rotationSize)) * rotation / rotationSize;

				const float newOrientation = oldOrientation + diff;

				if (isnan(targetOrientation) || isnan(newOrientation))
				{
					__debugbreak();
				}

				if (abs(AngleDifference(oldOrientation, targetOrientation)) < abs(AngleDifference(oldOrientation, newOrientation)))
				{
					GetOwner()->SetRotation(Quaternion::MakeRotationZ(targetOrientation));
				}
				else
				{
					GetOwner()->SetRotation(Quaternion::MakeRotationZ(newOrientation));
				}
			}
		}
	}
}
