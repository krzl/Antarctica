#pragma once

#include "SteeringBehavior.h"

namespace Navigation
{
	class AlignmentBehavior : public SteeringBehavior
	{
	public:

		explicit AlignmentBehavior(MovementComponent* movementComponent)
			: SteeringBehavior(movementComponent)
		{
			SetWeight(0.4f);
		}

		Vector2D GetLinearAcceleration() override;

		[[nodiscard]] float GetCohesionScale() const { return m_cohesionScale; }
		void                SetCohesionScale(const float cohesionScale) { m_cohesionScale = cohesionScale; }

	protected:

		float m_cohesionScale = 1.0f;
	};
}
