#pragma once

#include "GameObjects/Component.h"
#include "Steering/SteeringPipeline.h"

namespace Navigation
{
	class MovementComponent : public Component
	{
	private:

		DEFINE_CLASS()

		friend class SteeringBehavior;
		friend class ArriveBehavior;
		friend class MovementTester;

	public:

		MovementComponent();

		void MoveTo(Point3D targetPosition) const;

		Point3D GetTarget() const;
		bool    HasTarget() const;

		Dispatcher<> m_onArrive;

		[[nodiscard]] const Vector2D& GetVelocity() const { return m_velocity; }
		void                          SetVelocity(const Vector2D& velocity) { m_velocity = velocity; }
		[[nodiscard]] float           GetRadius() const { return m_radius; }
		void                          SetRadius(const float radius) { m_radius = radius; }
		[[nodiscard]] float           GetMaxAcceleration() const { return m_maxAcceleration; }
		void                          SetMaxAcceleration(const float maxAcceleration) { m_maxAcceleration = maxAcceleration; }
		[[nodiscard]] float           GetMaxSpeed() const { return m_maxSpeed; }
		void                          SetMaxSpeed(const float maxSpeed) { m_maxSpeed = maxSpeed; }
		[[nodiscard]] float           GetDecelerationFactor() const { return m_decelerationFactor; }
		void                          SetDecelerationFactor(const float decelerationFactor) { m_decelerationFactor = decelerationFactor; }

		[[nodiscard]] float GetMaxRotation() const { return m_maxRotation; }
		void                SetMaxRotation(const float maxRotation) { m_maxRotation = maxRotation; }

		BoundingBox GetBoundingBox() const override;

	protected:

		SteeringPipeline m_steeringPipeline;

		void Tick(float deltaTime) override;

		template<typename T, class = std::enable_if<std::is_base_of_v<SteeringBehavior, T>>>
		std::unique_ptr<T> CreateBehavior()
		{
			std::unique_ptr<T> behavior = std::make_unique<T>(this);
			m_steeringPipeline.m_behaviors.push_back(behavior.get());
			return behavior;
		}

	private:

		Vector2D m_velocity = Vector2D::zero;

		float m_radius = 0.0f;

		float m_maxAcceleration    = 100.0f;
		float m_maxSpeed           = 4.0f;
		float m_decelerationFactor = 2.0f;

		float m_maxRotation = 7.0f;

		std::vector<MovementComponent*> m_targets;

		std::unique_ptr<class ArriveBehavior>     m_arriveBehavior;
		std::unique_ptr<class SeparationBehavior> m_separationBehavior;
		std::unique_ptr<class CohesionBehavior>   m_cohesionBehavior;
		std::unique_ptr<class AlignmentBehavior>  m_alignmentBehavior;
	};

	CREATE_CLASS(MovementComponent)
}
