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

		Vector2D GetLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement,
									   const std::vector<NearbyTarget>& nearbyTargets) override;

		[[nodiscard]] float GetDecayCoefficient() const { return m_decayCoefficient; }
		void SetDecayCoefficient(const float radiusScale) { m_decayCoefficient = radiusScale; }

	private:

		float m_decayCoefficient = 0.075f;
	};
}
