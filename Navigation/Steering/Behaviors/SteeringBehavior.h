#pragma once

struct TransformComponent;

namespace Navigation
{
	struct NearbyTarget;
	struct MovementComponent;

	class SteeringBehavior
	{
	public:

		virtual ~SteeringBehavior() = default;

		virtual void InitializeTotalAccelerationCalculation(const TransformComponent* transform, MovementComponent* movement) = 0;
		virtual void UpdateNearbyEntity(const TransformComponent* transform, MovementComponent* movement,
										const TransformComponent* nearbyTransform, MovementComponent* nearbyMovement) = 0;
		virtual Vector2D GetFinalLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement) = 0;

		[[nodiscard]] float GetWeight() const { return m_weight; }
		void SetWeight(const float weight) { m_weight = weight; }

	private:

		float m_weight = 0.4f;
	};
}
