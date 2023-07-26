#pragma once

#include "Components/Component.h"
#include "Steering/SteeringPipeline.h"
#include "Steering/Behaviors/AlignmentBehavior.h"
#include "Steering/Behaviors/ArriveBehavior.h"
#include "Steering/Behaviors/CohesionBehavior.h"
#include "Steering/Behaviors/SeparationBehavior.h"

namespace Navigation
{
	struct MovementComponent : Component
	{
		Vector2D m_velocity = Vector2D::zero;

		float m_radius = 0.0f;

		float m_maxAcceleration    = 50.0f;
		float m_maxSpeed           = 4.0f;
		float m_decelerationFactor = 2.0f;

		float m_maxRotation = 7.0f;

		AlignmentBehavior m_alignmentBehavior;
		ArriveBehavior m_arriveBehavior;
		CohesionBehavior m_cohesionBehavior;
		SeparationBehavior m_separationBehavior;

		SteeringPipeline m_steeringPipeline;

		bool m_enableMovementTester = true;

		DEFINE_CLASS()
	};

	CREATE_CLASS(MovementComponent)
}
