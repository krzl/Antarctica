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

		void OnArrive(const Point3D   target,
			const TransformComponent* transform,
			const MovementComponent*  movement);

		Vector2D GetLinearAcceleration(const TransformComponent* transform,
			const MovementComponent*                             movement,
			const std::vector<NearbyTarget>&                     nearbyTargets) override;

		void    SetTarget(const Point3D& point);
		Point3D GetTarget() const { return m_target.value(); }
		bool    HasTarget() const { return m_target.has_value(); }
		void    ClearTarget() { m_target.reset(); }

		bool HasArrived() const { return m_hasArrived; }

		[[nodiscard]] float GetTargetRadius() const { return m_targetRadius; }
		void                SetTargetRadius(const float targetRadius) { m_targetRadius = targetRadius; }
		[[nodiscard]] float GetSlowdownRadius() const { return m_slowdownRadius; }
		void                SetSlowdownRadius(const float slowdownRadius) { m_slowdownRadius = slowdownRadius; }
		[[nodiscard]] float GetTimeToTarget() const { return m_timeToTarget; }
		void                SetTimeToTarget(const float timeToTarget) { m_timeToTarget = timeToTarget; }

	protected:

		std::optional<Point3D> m_target;

		bool  m_hasArrived;
		float m_targetRadius   = 1.5;
		float m_slowdownRadius = 0.0f;
		float m_timeToTarget   = 0.1f;
	};
}
