#pragma once

#include "GameObjects/GameObject.h"

namespace Navigation
{
	class MovementComponent;

	class MovementTester : public GameObject
	{
	public:

		MovementTester();

		void SetComponents(const std::vector<MovementComponent*>& components);

	protected:

		void Tick(float deltaTime) override;

	private:

		std::vector<MovementComponent*> m_components;

		float m_arriveWeight     = 0.0f;
		float m_separationWeight = 0.0f;
		float m_cohesionWeight   = 0.0f;
		float m_alignmentWeight  = 0.0f;

		float m_agentRadius        = 0.0f;
		float m_maxAcceleration    = 0.0f;
		float m_maxSpeed           = 0.0f;
		float m_decelerationFactor = 0.0f;

		float m_maxRotation            = 0.0f;

		float m_arriveTargetRadius      = 0.0f;
		float m_arriveTargetOuterRadius = 0.0f;
		float m_arriveNotifyRadius      = 0.0f;
		float m_arriveSlowdownRadius    = 0.0f;
		float m_arriveTimeToTarget      = 0.0f;

		float m_cohesionScale          = 0.0f;
		float m_alignmentCohesionScale = 0.0f;
		float m_separationDecayCoefficient  = 0.0f;

		bool m_showDebug = false;

		DEFINE_CLASS()
	};

	CREATE_CLASS(MovementTester)
}
