#pragma once

#include "SteeringBehavior.h"

namespace Navigation
{
	class AvoidanceBehavior : public SteeringBehavior
	{
	public:

		explicit AvoidanceBehavior()
		{
			SetWeight(0.0f);
		}

		void InitializeTotalAccelerationCalculation(const TransformComponent* transform, MovementComponent* movement) override;
		void UpdateNearbyEntity(const TransformComponent* transform, MovementComponent* movement, const TransformComponent* nearbyTransform,
								MovementComponent* nearbyMovement) override;
		Vector2D GetFinalLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement) override;

	private:

		const TransformComponent* m_closestTransform = nullptr;
		float m_closestDistanceSqr = 0.0f;
	};
}
