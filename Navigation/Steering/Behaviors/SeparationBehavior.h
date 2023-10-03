#pragma once

#include "SteeringBehavior.h"

namespace Navigation
{
	class SeparationBehavior : public SteeringBehavior
	{
	public:

		explicit SeparationBehavior()
		{
			SetWeight(1.0f);
		}

		void InitializeTotalAccelerationCalculation(const TransformComponent* transform, MovementComponent* movement) override;
		void UpdateNearbyEntity(const TransformComponent* transform, MovementComponent* movement, const TransformComponent* nearbyTransform,
								MovementComponent* nearbyMovement) override;
		Vector2D GetFinalLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement) override;

		[[nodiscard]] float GetDecayCoefficient() const { return m_decayCoefficient; }
		void SetDecayCoefficient(const float radiusScale) { m_decayCoefficient = radiusScale; }

	private:

		float m_decayCoefficient = 0.075f;

		Vector2D m_totalAcceleration = Vector2D::zero;
		uint32_t m_actorCount      = 0;
	};
}
