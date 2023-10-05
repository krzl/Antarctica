#pragma once

#include "SteeringBehavior.h"

namespace Navigation
{
	class ArriveBehavior : public SteeringBehavior
	{
	public:

		explicit ArriveBehavior()
		{
			SetWeight(0.4f);
		}

		Dispatcher<> m_onArrive;

		void InitializeTotalAccelerationCalculation(const TransformComponent* transform, MovementComponent* movement) override;
		void UpdateNearbyEntity(const TransformComponent* transform, MovementComponent* movement, const TransformComponent* nearbyTransform,
								MovementComponent* nearbyMovement) override;
		Vector2D GetFinalLinearAcceleration(const TransformComponent* transform, const MovementComponent* movement) override;

		void SetTarget(const Point3D& point, uint32_t delay = 0);
		Point3D GetTarget() const { return m_target.value_or(Point3D(0.0f, 0.0f, 0.0f)); }
		bool HasTarget() const { return m_target.has_value() && !m_hasArrived; }

		bool HasArrived() const { return m_hasArrived; }

		[[nodiscard]] float GetTargetRadius() const { return m_targetRadius; }
		void SetTargetRadius(const float targetRadius) { m_targetRadius = targetRadius; }

		[[nodiscard]] float GetOuterTargetRadius() const { return m_outerTargetRadius; }
		void SetOuterTargetRadius(const float outerTargetRadius) { m_outerTargetRadius = outerTargetRadius; }

	protected:

		bool HasArrivedCheck(const TransformComponent* transform);

		std::optional<Point3D> m_target;

		bool m_hasArrived = false;

		std::optional<std::list<uint32_t>> m_path;
		std::list<unsigned>::iterator m_currentPathSegment;

		uint32_t m_framesUntilCalculatePath = -1;

		float m_targetRadius      = 4.0f;
		float m_outerTargetRadius = 12.0f;
	};
}
