#pragma once

#include "Components/Component.h"
#include "Steering/SteeringPipeline.h"
#include "Steering/Behaviors/AlignmentBehavior.h"
#include "Steering/Behaviors/ArriveBehavior.h"
#include "Steering/Behaviors/AvoidanceBehavior.h"
#include "Steering/Behaviors/CohesionBehavior.h"
#include "Steering/Behaviors/SeparationBehavior.h"

namespace Physics
{
	class MovementSystem;
	class ApplyImpulseSystem;
}

namespace Navigation
{
	struct MovementComponent : Component
	{
		friend class Physics::MovementSystem;
		friend class Physics::ApplyImpulseSystem;

		Vector2D m_force    = Vector2D::zero;
		Vector2D m_velocity = Vector2D::zero;

		float m_radius = 0.0f;

		float m_maxAcceleration = 1500.0f;
		float m_maxSpeed        = 4.0f;

		float m_maxRotation = 7.0f;

		AlignmentBehavior m_alignmentBehavior;
		ArriveBehavior m_arriveBehavior;
		AvoidanceBehavior m_avoidanceBehavior;
		CohesionBehavior m_cohesionBehavior;
		SeparationBehavior m_separationBehavior;

		SteeringPipeline m_steeringPipeline;

		bool m_enableMovementTester = true;

	private:

		Vector2D m_positionCorrection = Vector2D();

		DEFINE_CLASS()
	};

	CREATE_CLASS(MovementComponent)
}
