#pragma once

struct TransformComponent;

namespace Navigation
{
	struct NearbyTarget;
	struct MovementComponent;
	class SteeringBehavior;

	class SteeringPipeline
	{
	public:

		Vector2D GetLinearAcceleration(const TransformComponent* transform, MovementComponent* movement,
									   const std::vector<NearbyTarget>& nearbyTargets) const;
	};
}
