#pragma once

#include "SteeringBehavior.h"

namespace Navigation
{
	class CohesionBehavior : public SteeringBehavior
	{
	public:

		explicit CohesionBehavior(MovementComponent* movementComponent)
			: SteeringBehavior(movementComponent)
		{
			SetWeight(0.0f);
		}

		[[nodiscard]] float GetCohesionScale() const { return m_cohesionScale; }
		void                SetCohesionScale(const float cohesionScale) { m_cohesionScale = cohesionScale; }

	protected:

		float m_cohesionScale = 10.0f;

		Vector2D GetLinearAcceleration() override;
	};
}
