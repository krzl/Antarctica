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

		virtual Vector2D GetLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement,
											   const std::vector<NearbyTarget>& nearbyTargets) = 0;

		[[nodiscard]] float GetWeight() const { return m_weight; }
		void SetWeight(const float weight) { m_weight = weight; }

	private:

		float m_weight = 0.4f;
	};
}
