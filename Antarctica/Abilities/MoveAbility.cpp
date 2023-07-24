#include "stdafx.h"
#include "MoveAbility.h"

#include "Components/AnimatedMeshComponent.h"
#include "Components/CameraComponent.h"
#include "Components/MovementComponent.h"
#include "Components/TransformComponent.h"
#include "Core/Application.h"
#include "Debug/DebugDrawManager.h"
#include "Entities/Camera.h"
#include "Input/InputManager.h"
#include "Steering/Behaviors/ArriveBehavior.h"

void MoveAbility::Init(Entity& entity)
{
	//TODO: ECS: move mouse picking somewhere else (perhaps to camera itself?)
	const MousePosition pos = InputManager::GetInstance()->GetMousePosition();

	const float ndcX = (2.0f * pos.first) / Application::Get().GetWindow().GetWidth() - 1.0f;
	const float ndcY = 1.0f - (2.0f * pos.second) / Application::Get().GetWindow().GetHeight();

	const Vector4D clipCoords = Vector4D(ndcX, ndcY, 1.0f, 1.0f);

	Rendering::Camera* camera = Rendering::Camera::Get();

	ComponentAccessor           cameraAccessor  = camera->GetComponentAccessor();
	TransformComponent*         cameraTransform = cameraAccessor.GetComponent<TransformComponent>();
	Rendering::CameraComponent* cameraComponent = cameraAccessor.GetComponent<Rendering::CameraComponent>();

	Matrix4D inversePerspectiveMatrix = Inverse(cameraComponent->m_perspectiveMatrix);
	Vector4D eyeCoordinates           = inversePerspectiveMatrix * clipCoords;
	eyeCoordinates /= eyeCoordinates.w;

	Matrix4D inverseViewMatrix = Inverse(cameraComponent->m_viewMatrix);
	Vector4D rayWorld          = inverseViewMatrix * eyeCoordinates;

	Vector3D direction = rayWorld.xyz - cameraTransform->m_localPosition;
	direction          = direction.Normalize();

	Ray ray = { cameraTransform->m_localPosition, direction };

	double  t = -ray.m_origin.z / ray.m_direction.z;
	Point3D intersectionPoint;
	intersectionPoint.x = ray.m_origin.x + t * ray.m_direction.x;
	intersectionPoint.y = ray.m_origin.y + t * ray.m_direction.y;
	intersectionPoint.z = ray.m_origin.z + t * ray.m_direction.z;

	m_target = intersectionPoint;
}

void MoveAbility::Start(Entity& entity)
{
	ComponentAccessor componentAccessor = entity.GetComponentAccessor();

	Navigation::MovementComponent* movementComponent     = componentAccessor.GetComponent<Navigation::MovementComponent>();
	Anim::AnimatedMeshComponent*   animatedMeshComponent = componentAccessor.GetComponent<Anim::AnimatedMeshComponent>();

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
