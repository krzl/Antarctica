#include "stdafx.h"
#include "InstantMoveActivator.h"

#include "Abilities/MoveAbility.h"
#include "Camera/PlayerCameraSystem.h"

#include "Components/MovementComponent.h"

#include "Core/Application.h"

InstantMoveActivator::InstantMoveActivator()
{
	m_triggerImmediately     = true;
	m_activateForAllSelected = true;
}

void InstantMoveActivator::Update()
{
	m_cursorPosition = Application::Get().GetSystem<PlayerCameraSystem>()->GetCursorWorldPosition();
}

bool InstantMoveActivator::CanBeFinished()
{
	return m_cursorPosition.has_value();
}

bool InstantMoveActivator::ShouldBeCancelled()
{
	return !m_cursorPosition.has_value();
}

std::shared_ptr<Ability> InstantMoveActivator::Activate(Entity* entity)
{
	const Navigation::MovementComponent* movement = entity->GetComponentAccessor().GetComponent<Navigation::MovementComponent>();
	const uint32_t selectedCount                  = Application::Get().GetSystem<PlayerCameraSystem>()->GetSelectedCount();

	uint32_t pathFindingDelay = m_entityActivationCount++ / NAVIGATION_MAX_UNITS_PER_FRAME;
	return std::make_shared<MoveAbility>(m_cursorPosition.value(), pathFindingDelay,
		Terathon::Sqrt(selectedCount * Max(movement->m_radius, movement->m_colliderRadius) / Terathon::Math::pi) * 1.1f);
}

void InstantMoveActivator::OnFinished()
{
	m_entityActivationCount = 0;
}
