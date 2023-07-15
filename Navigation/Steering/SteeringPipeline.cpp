#include "stdafx.h"
#include "SteeringPipeline.h"

#include "Behaviors/SteeringBehavior.h"
#include "Entities/MovementComponent.h"

namespace Navigation
{
	Vector2D SteeringPipeline::GetLinearAcceleration(const MovementComponent* movementComponent) const
	{
		Vector2D totalAcceleration = Vector2D::zero;

		for (SteeringBehavior* steeringBehavior : m_behaviors)
		{
			const float weight = steeringBehavior->GetWeight();

			if (weight == 0.0f)
			{
				continue;
			}

			const Vector2D acceleration = steeringBehavior->GetLinearAcceleration();

			if (IsNaN(acceleration))
			{
				continue;
			}

			totalAcceleration += acceleration * weight;
		}

		const float accelerationMagnitude = Magnitude(totalAcceleration);
		if (accelerationMagnitude > movementComponent->GetMaxAcceleration())
		{
			totalAcceleration = totalAcceleration * (movementComponent->GetMaxAcceleration() / accelerationMagnitude);
		}

		return totalAcceleration;
	}
}
