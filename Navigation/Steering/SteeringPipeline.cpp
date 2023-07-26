#include "stdafx.h"
#include "SteeringPipeline.h"

#include "Behaviors/SteeringBehavior.h"
#include "Components/MovementComponent.h"

namespace Navigation
{
	Vector2D SteeringPipeline::GetLinearAcceleration(const TransformComponent* transform, MovementComponent* movement,
													 const std::vector<NearbyTarget>& nearbyTargets) const
	{
		Vector2D totalAcceleration = Vector2D::zero;

		const std::vector<SteeringBehavior*> behaviors{
			&movement->m_arriveBehavior,
			&movement->m_alignmentBehavior,
			&movement->m_cohesionBehavior,
			&movement->m_separationBehavior
		};

		for (SteeringBehavior* steeringBehavior : behaviors)
		{
			const float weight = steeringBehavior->GetWeight();

			if (weight == 0.0f)
			{
				continue;
			}

			const Vector2D acceleration = steeringBehavior->GetLinearAcceleration(transform, movement, nearbyTargets);

			if (IsNaN(acceleration))
			{
				continue;
			}

			totalAcceleration += acceleration * weight;
		}

		const float accelerationMagnitude = Magnitude(totalAcceleration);
		if (accelerationMagnitude > movement->m_maxAcceleration)
		{
			totalAcceleration = totalAcceleration * (movement->m_maxAcceleration / accelerationMagnitude);
		}

		return totalAcceleration;
	}
}
