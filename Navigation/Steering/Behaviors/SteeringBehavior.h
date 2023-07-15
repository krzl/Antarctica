#pragma once

namespace Navigation
{
	class MovementComponent;

	class SteeringBehavior
	{
	public:

		explicit SteeringBehavior(MovementComponent* movementComponent);

		virtual ~SteeringBehavior() = default;

		virtual Vector2D GetLinearAcceleration() = 0;

		[[nodiscard]] float GetWeight() const { return m_weight; }
		void                SetWeight(const float weight) { m_weight = weight; }

		const std::vector<MovementComponent*>& GetCachedTargets() const;

	protected:

		MovementComponent* m_movement = nullptr;

	private:
		
		float m_weight = 0.4f;
	};
}
