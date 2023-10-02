#include "stdafx.h"
#include "MoveAbility.h"

#include "Camera/PlayerCameraSystem.h"
#include "Components/AnimatedMeshComponent.h"
#include "Components/MovementComponent.h"
#include "Core/Application.h"

#include "Debug/DebugDrawManager.h"

#include "Entities/Camera.h"

#include "Pathfinding/PathFinding.h"

#include "Steering/Behaviors/ArriveBehavior.h"

MoveAbility::MoveAbility(const Point3D& target) :
	m_target(target) {}

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

	movementComponent->m_arriveBehavior.SetTarget(m_target);
	animatedMeshComponent->m_animationSolver.SetTrigger(Anim::AnimTrigger::WALK, true);

	const Point3D& currentPosition = componentAccessor.GetComponent<TransformComponent>()->m_localPosition;

	std::optional<std::list<uint32_t>> path = Navigation::PathFinding::FindPath(currentPosition, m_target);

	const Navigation::NavMesh& navMesh = *Application::Get().GetGameState().GetNavMesh();

#if defined(DEBUG) | defined(_DEBUG)
	if (!path.has_value())
	{
		return;
	}
	if (path->empty())
	{
		DebugDrawManager::GetInstance()->DrawLine(currentPosition + Vector3D(0.0f, 0.0f, 1.0f),
			m_target + Vector3D(0.0f, 0.0f, 1.0f), 0.1f, 10000.0f);
	}
	else
	{
		Point3D previousPoint = currentPosition;
		while (!path->empty())
		{
			Point3D nextPoint = navMesh.GetVertexPosition(path->front());
			DebugDrawManager::GetInstance()->DrawLine(previousPoint + Vector3D(0.0f, 0.0f, 1.0f),
				nextPoint + Vector3D(0.0f, 0.0f, 1.0f), 0.1f, 10000.0f);

			previousPoint = nextPoint;

			path->remove(path->front());
		}
		DebugDrawManager::GetInstance()->DrawLine(previousPoint + Vector3D(0.0f, 0.0f, 1.0f),
			m_target + Vector3D(0.0f, 0.0f, 1.0f), 0.1f, 10000.0f);
	}
#endif
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

	Navigation::MovementComponent* movementComponent = componentAccessor.GetComponent<Navigation::MovementComponent>();
	movementComponent->m_arriveBehavior.ClearTarget();

	Anim::AnimatedMeshComponent* animatedMeshComponent = componentAccessor.GetComponent<Anim::AnimatedMeshComponent>();
	animatedMeshComponent->m_animationSolver.SetTrigger(Anim::AnimTrigger::WALK, false);
}
