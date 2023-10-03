﻿#pragma once

#include "SteeringBehavior.h"

namespace Navigation
{
	class AlignmentBehavior : public SteeringBehavior
	{
	public:

		explicit AlignmentBehavior()
		{
			SetWeight(0.4f);
		}

		void InitializeTotalAccelerationCalculation(const TransformComponent* transform, MovementComponent* movement) override;
		void UpdateNearbyEntity(const TransformComponent* transform, MovementComponent* movement, const TransformComponent* nearbyTransform,
								MovementComponent* nearbyMovement) override;
		Vector2D GetFinalLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement) override;

		[[nodiscard]] float GetCohesionScale() const { return m_cohesionScale; }
		void SetCohesionScale(const float cohesionScale) { m_cohesionScale = cohesionScale; }

	protected:

		float m_cohesionScale = 1.0f;

	private:
		
		Vector2D m_heading = Vector2D::zero;
		uint32_t m_actorCount = 0;
	};
}
