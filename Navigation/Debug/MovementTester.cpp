#include "stdafx.h"
#include "MovementTester.h"

#include "Components/MovementComponent.h"
#include "Components/TransformComponent.h"
#include "Debug/DebugDrawManager.h"
#include "imgui/imgui.h"

namespace Navigation
{
	void MovementTester::DrawMenu()
	{
		if (!m_hasComponentsToTest)
		{
			return;
		}

		m_hasValuesChanged    = false;
		m_hasComponentsToTest = false;

		ImGui::Begin("Agent steering");

		m_hasValuesChanged |= ImGui::SliderFloat("Arrive::Weight", &m_arriveWeight, 0.0f, 2.0f);
		m_hasValuesChanged |= ImGui::SliderFloat("Separation::Weight", &m_separationWeight, 0.0f, 2.0f);
		m_hasValuesChanged |= ImGui::SliderFloat("Cohesion::Weight", &m_cohesionWeight, 0.0f, 2.0f);
		m_hasValuesChanged |= ImGui::SliderFloat("Alignment::Weight", &m_alignmentWeight, 0.0f, 2.0f);
		ImGui::NewLine();
		m_hasValuesChanged |= ImGui::SliderFloat("Steering::AgentRadius", &m_agentRadius, 0.3f, 5.0f);
		m_hasValuesChanged |= ImGui::SliderFloat("Steering::MaxAcceleration", &m_maxAcceleration, 0.0f, 500.0f);
		m_hasValuesChanged |= ImGui::SliderFloat("Steering::MaxSpeed", &m_maxSpeed, 0.0f, 10.0f);
		ImGui::NewLine();
		m_hasValuesChanged |= ImGui::SliderFloat("Steering::MaxRotation", &m_maxRotation, 0.1f, 20.0f);
		ImGui::NewLine();
		m_hasValuesChanged |= ImGui::SliderFloat("Arrive::TargetRadius", &m_arriveTargetRadius, 0.0f, 5.0f);
		m_hasValuesChanged |= ImGui::SliderFloat("Arrive::SlowdownRadius", &m_arriveSlowdownRadius, 0.0f, 5.0f);
		m_hasValuesChanged |= ImGui::SliderFloat("Arrive::TimeToTarget", &m_arriveTimeToTarget, 0.0f, 5.0f);
		ImGui::NewLine();
		m_hasValuesChanged |= ImGui::SliderFloat("Cohesion::CohesionRange", &m_cohesionScale, 1.0f, 20.0f);
		m_hasValuesChanged |= ImGui::SliderFloat("Alignment::CohesionRange", &m_cohesionScale, 1.0f, 20.0f);
		m_hasValuesChanged |= ImGui::SliderFloat("Separation::DecayCoefficient", &m_separationDecayCoefficient, 0.001f, 1.0f);
		ImGui::NewLine();
		ImGui::Checkbox("ShowDebug", &m_showDebug);
		ImGui::End();

		m_hasComponentsToTest = false;
	}

	void MovementTester::UpdateComponent(MovementComponent* movement, TransformComponent* transform)
	{
		m_hasComponentsToTest = true;

		const bool shouldInitialize = !m_hasBeenInitialized.exchange(true);
		if (shouldInitialize)
		{
			m_arriveWeight     = movement->m_arriveBehavior.GetWeight();
			m_separationWeight = movement->m_separationBehavior.GetWeight();
			m_cohesionWeight   = movement->m_cohesionBehavior.GetWeight();
			m_alignmentWeight  = movement->m_alignmentBehavior.GetWeight();

			m_agentRadius     = movement->m_radius;
			m_maxAcceleration = movement->m_maxAcceleration;
			m_maxSpeed        = movement->m_maxSpeed;

			m_maxRotation = movement->m_maxRotation;

			m_arriveTargetRadius   = movement->m_arriveBehavior.GetTargetRadius();
			m_arriveSlowdownRadius = movement->m_arriveBehavior.GetSlowdownRadius();
			m_arriveTimeToTarget   = movement->m_arriveBehavior.GetTimeToTarget();

			m_cohesionScale              = movement->m_cohesionBehavior.GetCohesionScale();
			m_separationDecayCoefficient = movement->m_separationBehavior.GetDecayCoefficient();
			m_alignmentCohesionScale     = movement->m_alignmentBehavior.GetCohesionScale();
		}

		if (m_hasValuesChanged)
		{
			movement->m_arriveBehavior.SetWeight(m_arriveWeight);
			movement->m_separationBehavior.SetWeight(m_separationWeight);
			movement->m_cohesionBehavior.SetWeight(m_cohesionWeight);
			movement->m_alignmentBehavior.SetWeight(m_alignmentWeight);

			movement->m_radius          = m_agentRadius;
			movement->m_maxAcceleration = m_maxAcceleration;
			movement->m_maxSpeed        = m_maxSpeed;

			movement->m_maxRotation = m_maxRotation;

			movement->m_arriveBehavior.SetTargetRadius(m_arriveTargetRadius);
			movement->m_arriveBehavior.SetSlowdownRadius(m_arriveSlowdownRadius);
			movement->m_arriveBehavior.SetTimeToTarget(m_arriveTimeToTarget);

			movement->m_cohesionBehavior.SetCohesionScale(m_cohesionScale);
			movement->m_separationBehavior.SetDecayCoefficient(m_separationDecayCoefficient);
			movement->m_alignmentBehavior.SetCohesionScale(m_alignmentCohesionScale);
		}

		if (m_showDebug)
		{
			static std::mutex debugDrawMutex;
			std::lock_guard   lock(debugDrawMutex);

			const float speedRatio = Magnitude(movement->m_velocity) / movement->m_maxSpeed;
			const Color color      = LerpClamped(Color::red, Color::white, speedRatio);

			DebugDrawManager::GetInstance()->DrawSphere(transform->m_localPosition, movement->m_radius, 0.0f, color, 8);
			DebugDrawManager::GetInstance()->DrawLine(transform->m_localPosition,
													  transform->m_localPosition + Vector3D(movement->m_velocity, 0.0f),
													  0.05f,
													  0.0f,
													  Color::yellow,
													  6);
		}
	}
}
