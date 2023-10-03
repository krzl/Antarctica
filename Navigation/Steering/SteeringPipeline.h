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

		void InitializeTotalAccelerationCalculation(const TransformComponent* transform, MovementComponent* movement) const;
		void UpdateNearbyEntity(const TransformComponent* transform, MovementComponent* movement,
								const TransformComponent* nearbyTransform, MovementComponent* nearbyMovement) const;
		Vector2D GetFinalLinearAcceleration(const TransformComponent* transform, MovementComponent* movement) const;

	private:

		static void InitializeTotalAccelerationCalculation(SteeringBehavior& steeringBehavior, const TransformComponent* transform,
														   MovementComponent* movement);
		static void UpdateNearbyEntity(SteeringBehavior& steeringBehavior, const TransformComponent* transform, MovementComponent* movement,
									   const TransformComponent* nearbyTransform, MovementComponent* nearbyMovement);
		Vector2D GetLinearAcceleration(SteeringBehavior& steeringBehavior, const TransformComponent* transform,
									   const MovementComponent* movement) const;
	};
}
