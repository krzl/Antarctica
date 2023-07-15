#pragma once

#include "SteeringBehavior.h"

namespace Navigation
{
	class ArriveBehavior : public SteeringBehavior
	{
	public:

		explicit ArriveBehavior(MovementComponent* movementComponent)
			: SteeringBehavior(movementComponent)
		{
			SetWeight(0.4f);
		}

		Dispatcher<> m_onArrive;

		void     OnArrive();
		Vector2D GetLinearAcceleration() override;

		void    SetTarget(const Point3D& point) { m_target = point; }
		Point3D GetTarget() { return m_target.value(); }
		bool    HasTarget() const { return m_target.has_value(); }

		[[nodiscard]] float GetTargetRadius() const { return m_targetRadius; }
		void                SetTargetRadius(const float targetRadius) { m_targetRadius = targetRadius; }
		[[nodiscard]] float GetTargetOuterRadius() const { return m_targetOuterRadius; }
		void                SetTargetOuterRadius(const float targetOuterRadius) { m_targetOuterRadius = targetOuterRadius; }
		[[nodiscard]] float GetNotifyRadius() const { return m_notifyRadius; }
		void                SetNotifyRadius(const float notifyRadius) { m_notifyRadius = notifyRadius; }
		[[nodiscard]] float GetSlowdownRadius() const { return m_slowdownRadius; }
		void                SetSlowdownRadius(const float slowdownRadius) { m_slowdownRadius = slowdownRadius; }
		[[nodiscard]] float GetTimeToTarget() const { return m_timeToTarget; }
		void                SetTimeToTarget(const float timeToTarget) { m_timeToTarget = timeToTarget; }

	protected:

		std::optional<Point3D> m_target;

		float m_targetRadius      = 1.5;
		float m_targetOuterRadius = 3.0;
		float m_notifyRadius      = 0.0f;
		float m_slowdownRadius    = 0.0f;
		float m_timeToTarget      = 0.1f;
	};
}
