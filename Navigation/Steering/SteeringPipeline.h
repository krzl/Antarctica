#pragma once

namespace Navigation
{
	class MovementComponent;
	class SteeringBehavior;

	class SteeringPipeline
	{
	public:

		std::vector<SteeringBehavior*> m_behaviors;

		Vector2D GetLinearAcceleration(const MovementComponent* movementComponent) const;
	};
}
