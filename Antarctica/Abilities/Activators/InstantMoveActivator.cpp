#include "stdafx.h"
#include "InstantMoveActivator.h"

#include "Abilities/MoveAbility.h"
#include "Camera/PlayerCameraSystem.h"
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
	uint32_t pathFindingDelay = m_entityActivationCount++ / NAVIGATION_MAX_UNITS_PER_FRAME;
	return std::make_shared<MoveAbility>(m_cursorPosition.value(), pathFindingDelay);
}

void InstantMoveActivator::OnFinished()
{
	m_entityActivationCount = 0;
}