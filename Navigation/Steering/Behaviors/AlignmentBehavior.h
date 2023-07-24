#pragma once

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

		Vector2D GetLinearAcceleration(const TransformComponent* transform,
			const MovementComponent*                             movement,
			const std::vector<NearbyTarget>&                     nearbyTargets) override;

		[[nodiscard]] float GetCohesionScale() const { return m_cohesionScale; }
		void                SetCohesionScale(const float cohesionScale) { m_cohesionScale = cohesionScale; }

	protected:

		float m_cohesionScale = 1.0f;
	};
}
