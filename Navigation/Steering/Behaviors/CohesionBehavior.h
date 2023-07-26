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

		Vector2D GetLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement,
									   const std::vector<NearbyTarget>& nearbyTargets) override;

		[[nodiscard]] float GetCohesionScale() const { return m_cohesionScale; }
		void SetCohesionScale(const float cohesionScale) { m_cohesionScale = cohesionScale; }

	protected:

		float m_cohesionScale = 10.0f;
	};
}
