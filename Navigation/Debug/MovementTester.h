#pragma once

struct TransformComponent;

namespace Navigation
{
	struct MovementComponent;

	class MovementTester
	{
	public:
		
		void DrawMenu();
		void UpdateComponent(MovementComponent* movement, TransformComponent* transform);

		float m_arriveWeight     = 0.0f;
		float m_separationWeight = 0.0f;
		float m_cohesionWeight   = 0.0f;
		float m_alignmentWeight  = 0.0f;

		float m_agentRadius        = 0.0f;
		float m_maxAcceleration    = 0.0f;
		float m_maxSpeed           = 0.0f;

		float m_maxRotation = 0.0f;

		float m_arriveTargetRadius      = 0.0f;
		float m_arriveSlowdownRadius    = 0.0f;
		float m_arriveTimeToTarget      = 0.0f;

		float m_cohesionScale              = 0.0f;
		float m_alignmentCohesionScale     = 0.0f;
		float m_separationDecayCoefficient = 0.0f;

		bool m_showDebug = false;
		bool m_hasValuesChanged = false;

		std::atomic_bool m_hasComponentsToTest = false;
		std::atomic_bool m_hasBeenInitialized = false;
	};
}
