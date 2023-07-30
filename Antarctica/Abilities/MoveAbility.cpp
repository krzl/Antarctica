#include "stdafx.h"
#include "MoveAbility.h"

#include "Camera/PlayerCameraSystem.h"
#include "Components/AnimatedMeshComponent.h"
#include "Components/MovementComponent.h"
#include "Core/Application.h"
#include "Entities/Camera.h"
#include "Steering/Behaviors/ArriveBehavior.h"

bool MoveAbility::Init(Entity& entity)
{
	const std::optional<Point3D>& cursorWorldPosition = Application::Get().GetSystem<PlayerCameraSystem>()->GetCursorWorldPosition();
	if (cursorWorldPosition.has_value())
	{
		m_target = cursorWorldPosition.value();
		return true;
	}

	return false;
}

void MoveAbility::Start(Entity& entity)
{
	ComponentAccessor componentAccessor = entity.GetComponentAccessor();

	Navigation::MovementComponent* movementComponent   = componentAccessor.GetComponent<Navigation::MovementComponent>();
	Anim::AnimatedMeshComponent* animatedMeshComponent = componentAccessor.GetComponent<Anim::AnimatedMeshComponent>();

	movementComponent->m_arriveBehavior.SetTarget(m_target);
	animatedMeshComponent->m_animationSolver.SetTrigger(Anim::AnimTrigger::WALK, true);
}

bool MoveAbility::Update(Entity& entity)
{
	ComponentAccessor componentAccessor = entity.GetComponentAccessor();

	const Navigation::MovementComponent* movementComponent = componentAccessor.GetComponent<Navigation::MovementComponent>();
	return movementComponent->m_arriveBehavior.HasArrived();
}

void MoveAbility::End(Entity& entity)
{
	ComponentAccessor componentAccessor = entity.GetComponentAccessor();

	Navigation::MovementComponent* movementComponent = componentAccessor.GetComponent<Navigation::MovementComponent>();
	movementComponent->m_arriveBehavior.ClearTarget();

	Anim::AnimatedMeshComponent* animatedMeshComponent = componentAccessor.GetComponent<Anim::AnimatedMeshComponent>();
	animatedMeshComponent->m_animationSolver.SetTrigger(Anim::AnimTrigger::WALK, false);
}
