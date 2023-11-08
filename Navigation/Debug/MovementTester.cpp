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

		ImGui::Begin("Agent steering");

		m_hasValuesChanged |= ImGui::SliderFloat("Arrive::Weight", &m_arriveWeight, 0.0f, 2.0f);
		m_hasValuesChanged |= ImGui::SliderFloat("Separation::Weight", &m_separationWeight, 0.0f, 2.0f);
		m_hasValuesChanged |= ImGui::SliderFloat("Cohesion::Weight", &m_cohesionWeight, 0.0f, 2.0f);
		m_hasValuesChanged |= ImGui::SliderFloat("Alignment::Weight", &m_alignmentWeight, 0.0f, 2.0f);
		ImGui::NewLine();
		m_hasValuesChanged |= ImGui::SliderFloat("Steering::AgentRadius", &m_agentRadius, 0.3f, 5.0f);
		m_hasValuesChanged |= ImGui::SliderFloat("Steering::Acceleration", &m_acceleration, 200.0f, 10000.0f);
		m_hasValuesChanged |= ImGui::SliderFloat("Steering::MaxSpeed", &m_maxSpeed, 0.0f, 10.0f);
		ImGui::NewLine();
		m_hasValuesChanged |= ImGui::SliderFloat("Steering::MaxRotation", &m_maxRotation, 0.1f, 20.0f);
		ImGui::NewLine();
		m_hasValuesChanged |= ImGui::SliderFloat("Arrive::TargetRadius", &m_arriveTargetRadius, 0.0f, 10.0f);
		m_hasValuesChanged |= ImGui::SliderFloat("Arrive::OuterTargetRadius", &m_outerTargetRadius, 0.0f, 20.0f);
		ImGui::NewLine();
		m_hasValuesChanged |= ImGui::SliderFloat("Cohesion::CohesionRange", &m_cohesionScale, 1.0f, 20.0f);
		m_hasValuesChanged |= ImGui::SliderFloat("Alignment::CohesionRange", &m_cohesionScale, 1.0f, 20.0f);
		m_hasValuesChanged |= ImGui::SliderFloat("Separation::DecayCoefficient", &m_separationDecayCoefficient, 0.001f, 1.0f);
		ImGui::NewLine();
		ImGui::Checkbox("ShowDebug", &m_showDebug);
		ImGui::End();
	}

	void MovementTester::UpdateComponent(MovementComponent* movement, const TransformComponent* transform)
	{
		m_hasComponentsToTest = true;

		const bool shouldInitialize = !m_hasBeenInitialized.exchange(true);
		if (shouldInitialize)
		{
			m_arriveWeight     = movement->m_arriveBehavior.GetWeight();
			m_separationWeight = movement->m_separationBehavior.GetWeight();
			m_cohesionWeight   = movement->m_cohesionBehavior.GetWeight();
			m_alignmentWeight  = movement->m_alignmentBehavior.GetWeight();

			m_agentRadius  = movement->m_colliderRadius;
			m_acceleration = movement->m_maxAcceleration;
			m_maxSpeed     = movement->m_maxSpeed;

			m_maxRotation = movement->m_maxRotation;

			m_arriveTargetRadius = movement->m_arriveBehavior.GetTargetRadius();
			m_outerTargetRadius  = movement->m_arriveBehavior.GetOuterTargetRadius();

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

			movement->m_colliderRadius          = m_agentRadius;
			movement->m_maxAcceleration = m_acceleration;
			movement->m_maxSpeed        = m_maxSpeed;

			movement->m_maxRotation = m_maxRotation;

			movement->m_arriveBehavior.SetTargetRadius(m_arriveTargetRadius);
			movement->m_arriveBehavior.SetOuterTargetRadius(m_outerTargetRadius);

			movement->m_cohesionBehavior.SetCohesionScale(m_cohesionScale);
			movement->m_separationBehavior.SetDecayCoefficient(m_separationDecayCoefficient);
			movement->m_alignmentBehavior.SetCohesionScale(m_alignmentCohesionScale);
		}

		if (m_showDebug)
		{
			static std::mutex debugDrawMutex;
			std::lock_guard lock(debugDrawMutex);

			const float speedRatio = Magnitude(movement->m_velocity) / movement->m_maxSpeed;
			const Color color      = LerpClamped(Color::red, Color::white, speedRatio);

			DebugDrawManager::GetInstance()->DrawSphere(transform->m_localPosition, movement->m_colliderRadius, 0.0f, color, 8);
			DebugDrawManager::GetInstance()->DrawLine(transform->m_localPosition, transform->m_localPosition + Vector3D(movement->m_velocity, 0.0f),
				0.05f, 0.0f, Color::yellow, 6);
		}
	}
}
