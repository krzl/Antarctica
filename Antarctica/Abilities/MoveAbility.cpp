#include "stdafx.h"
#include "MoveAbility.h"

#include "Components/AnimatedMeshComponent.h"
#include "Components/MovementComponent.h"
#include "Entities/Camera.h"
#include "Steering/Behaviors/ArriveBehavior.h"

MoveAbility::MoveAbility(const Point3D& target, const uint32_t delay) :
	m_target(target),
	m_delay(delay) {}

bool MoveAbility::Init(Entity& entity)
{
	m_entity = &entity;
	return true;
}

void MoveAbility::Start()
{
	const ComponentAccessor& componentAccessor = m_entity->GetComponentAccessor();

	Navigation::MovementComponent* movementComponent   = componentAccessor.GetComponent<Navigation::MovementComponent>();
	Anim::AnimatedMeshComponent* animatedMeshComponent = componentAccessor.GetComponent<Anim::AnimatedMeshComponent>();

	movementComponent->m_arriveBehavior.SetTarget(m_target, m_delay);
	animatedMeshComponent->m_animationSolver.SetTrigger(Anim::AnimTrigger::WALK, true);
}

bool MoveAbility::Update()
{
	const ComponentAccessor& componentAccessor = m_entity->GetComponentAccessor();

	const Navigation::MovementComponent* movementComponent = componentAccessor.GetComponent<Navigation::MovementComponent>();
	return movementComponent->m_arriveBehavior.HasArrived();
}

void MoveAbility::End()
{
	const ComponentAccessor& componentAccessor = m_entity->GetComponentAccessor();

	Anim::AnimatedMeshComponent* animatedMeshComponent = componentAccessor.GetComponent<Anim::AnimatedMeshComponent>();
	animatedMeshComponent->m_animationSolver.SetTrigger(Anim::AnimTrigger::WALK, false);
}
