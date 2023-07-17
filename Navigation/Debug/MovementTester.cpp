#include "stdafx.h"
#include "MovementTester.h"

#include "Debug/DebugDrawManager.h"
#include "Entities/MovementComponent.h"
#include "imgui/imgui.h"
#include "Steering/Behaviors/AlignmentBehavior.h"
#include "Steering/Behaviors/ArriveBehavior.h"
#include "Steering/Behaviors/CohesionBehavior.h"
#include "Steering/Behaviors/SeparationBehavior.h"

namespace Navigation
{
	MovementTester::MovementTester()
	{
		m_isTickable = true;
	}

	void MovementTester::SetComponents(const std::vector<MovementComponent*>& components)
	{
		m_components = components;

		if (m_components.size() > 0)
		{
			const MovementComponent* comp = m_components[0];

			m_arriveWeight     = comp->m_arriveBehavior->GetWeight();
			m_separationWeight = comp->m_separationBehavior->GetWeight();
			m_cohesionWeight   = comp->m_cohesionBehavior->GetWeight();
			m_alignmentWeight  = comp->m_alignmentBehavior->GetWeight();

			m_agentRadius        = comp->GetRadius();
			m_maxAcceleration    = comp->GetMaxAcceleration();
			m_maxSpeed           = comp->GetMaxSpeed();
			m_decelerationFactor = comp->GetDecelerationFactor();

			m_maxRotation = comp->GetMaxRotation();

			m_arriveTargetRadius      = comp->m_arriveBehavior->GetTargetRadius();
			m_arriveTargetOuterRadius = comp->m_arriveBehavior->GetTargetOuterRadius();
			m_arriveNotifyRadius      = comp->m_arriveBehavior->GetNotifyRadius();
			m_arriveSlowdownRadius    = comp->m_arriveBehavior->GetSlowdownRadius();
			m_arriveTimeToTarget      = comp->m_arriveBehavior->GetTimeToTarget();

			m_cohesionScale              = comp->m_cohesionBehavior->GetCohesionScale();
			m_separationDecayCoefficient = comp->m_separationBehavior->GetDecayCoefficient();
			m_alignmentCohesionScale     = comp->m_alignmentBehavior->GetCohesionScale();
		}
	}

	void MovementTester::Tick(const float deltaTime)
	{
		if (m_components.size() == 0)
		{
			return;
		}

		bool hasValuesChanged = false;

		ImGui::Begin("Agent steering");

		hasValuesChanged |= ImGui::SliderFloat("Arrive::Weight", &m_arriveWeight, 0.0f, 2.0f);
		hasValuesChanged |= ImGui::SliderFloat("Separation::Weight", &m_separationWeight, 0.0f, 2.0f);
		hasValuesChanged |= ImGui::SliderFloat("Cohesion::Weight", &m_cohesionWeight, 0.0f, 2.0f);
		hasValuesChanged |= ImGui::SliderFloat("Alignment::Weight", &m_alignmentWeight, 0.0f, 2.0f);
		ImGui::NewLine();
		hasValuesChanged |= ImGui::SliderFloat("Steering::AgentRadius", &m_agentRadius, 0.3f, 5.0f);
		hasValuesChanged |= ImGui::SliderFloat("Steering::MaxAcceleration", &m_maxAcceleration, 0.0f, 500.0f);
		hasValuesChanged |= ImGui::SliderFloat("Steering::MaxSpeed", &m_maxSpeed, 0.0f, 10.0f);
		hasValuesChanged |= ImGui::SliderFloat("Steering::DecelerationFactor", &m_decelerationFactor, 0.0f, 10.0f);
		ImGui::NewLine();
		hasValuesChanged |= ImGui::SliderFloat("Steering::MaxRotation", &m_maxRotation, 0.1f, 20.0f);
		ImGui::NewLine();
		hasValuesChanged |= ImGui::SliderFloat("Arrive::TargetRadius", &m_arriveTargetRadius, 0.0f, 5.0f);
		hasValuesChanged |= ImGui::SliderFloat("Arrive::OuterTargetRadius", &m_arriveTargetOuterRadius, 0.0f, 5.0f);
		hasValuesChanged |= ImGui::SliderFloat("Arrive::NotifyRadius", &m_arriveNotifyRadius, 0.0f, 5.0f);
		hasValuesChanged |= ImGui::SliderFloat("Arrive::SlowdownRadius", &m_arriveSlowdownRadius, 0.0f, 5.0f);
		hasValuesChanged |= ImGui::SliderFloat("Arrive::TimeToTarget", &m_arriveTimeToTarget, 0.0f, 5.0f);
		ImGui::NewLine();
		hasValuesChanged |= ImGui::SliderFloat("Cohesion::CohesionRange", &m_cohesionScale, 1.0f, 20.0f);
		hasValuesChanged |= ImGui::SliderFloat("Alignment::CohesionRange", &m_cohesionScale, 1.0f, 20.0f);
		hasValuesChanged |= ImGui::SliderFloat("Separation::DecayCoefficient", &m_separationDecayCoefficient, 0.001f, 1.0f);
		ImGui::NewLine();
		ImGui::Checkbox("ShowDebug", &m_showDebug);
		ImGui::End();

		if (hasValuesChanged)
		{
			for (MovementComponent* comp : m_components)
			{
				comp->SetRadius(m_agentRadius);

				comp->m_arriveBehavior->SetWeight(m_arriveWeight);
				comp->m_separationBehavior->SetWeight(m_separationWeight);
				comp->m_cohesionBehavior->SetWeight(m_cohesionWeight);
				comp->m_alignmentBehavior->SetWeight(m_alignmentWeight);

				comp->SetMaxAcceleration(m_maxAcceleration);
				comp->SetMaxSpeed(m_maxSpeed);
				comp->SetDecelerationFactor(m_decelerationFactor);

				comp->SetMaxRotation(m_maxRotation);

				comp->m_arriveBehavior->SetTargetRadius(m_arriveTargetRadius);
				comp->m_arriveBehavior->SetTargetOuterRadius(m_arriveTargetOuterRadius);
				comp->m_arriveBehavior->SetNotifyRadius(m_arriveNotifyRadius);
				comp->m_arriveBehavior->SetSlowdownRadius(m_arriveSlowdownRadius);
				comp->m_arriveBehavior->SetTimeToTarget(m_arriveTimeToTarget);

				comp->m_cohesionBehavior->SetCohesionScale(m_cohesionScale);
				comp->m_separationBehavior->SetDecayCoefficient(m_separationDecayCoefficient);
				comp->m_alignmentBehavior->SetCohesionScale(m_alignmentCohesionScale);
			}
		}


		if (m_showDebug)
		{
			for (const MovementComponent* comp : m_components)
			{
				const float speedRatio = Magnitude(comp->GetVelocity()) / comp->GetMaxSpeed();
				const Color color      = LerpClamped(Color::red, Color::white, speedRatio);

				DebugDrawManager::GetInstance()->DrawSphere(comp->GetOwner()->GetPosition(), comp->GetRadius(), 0.0f, color, 8);
				DebugDrawManager::GetInstance()->DrawLine(comp->GetOwner()->GetPosition(), comp->GetOwner()->GetPosition() + Vector3D(comp->GetVelocity(), 0.0f), 0.05f, 0.0f, Color::yellow, 6);
			}
		}
	}
}
