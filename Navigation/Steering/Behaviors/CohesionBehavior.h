#pragma once

#include "SteeringBehavior.h"

namespace Navigation
{
	class CohesionBehavior : public SteeringBehavior
	{
	public:

		explicit CohesionBehavior()
		{
			SetWeight(0.0f);
		}

		void InitializeTotalAccelerationCalculation(const TransformComponent* transform, MovementComponent* movement) override;
		void UpdateNearbyEntity(const TransformComponent* transform, MovementComponent* movement, const TransformComponent* nearbyTransform,
								MovementComponent* nearbyMovement) override;
		Vector2D GetFinalLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement) override;

		[[nodiscard]] float GetCohesionScale() const { return m_cohesionScale; }
		void SetCohesionScale(const float cohesionScale) { m_cohesionScale = cohesionScale; }

	protected:

		float m_cohesionScale = 2.0f;

	private:

		Vector2D m_heading    = Vector2D::zero;
		uint32_t m_actorCount = 0;
	};
}
