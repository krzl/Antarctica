#include "stdafx.h"
#include "Application.h"

#include "Abilities/AbilitySystem.h"
#include "Camera/PlayerCameraSystem.h"
#include "Debug/DebugDraw/DebugDrawSystem.h"
#include "Debug/ImGui/ImGuiSystem.h"
#include "Steering/SteeringSystem.h"
#include "Systems/AnimationSystem.h"
#include "Systems/ApplyImpulseSystem.h"
#include "Systems/CollisionGatherSystem.h"
#include "Systems/CullingSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/PerformanceMonitorSystem.h"
#include "Systems/QuadtreeUpdateSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/SkinningSystem.h"
#include "Systems/TransformUpdateSystem.h"

void Application::CreateSystems()
{
	m_ecs.AddSystem<PerformanceMonitorSystem>(ECS::PRE_STEP_LOCK);
	m_ecs.AddSystem<PlayerCameraSystem>(ECS::PRE_STEP_LOCK);

	m_ecs.AddSystem<AbilitySystem>(ECS::STEP_LOCK);
	m_ecs.AddSystem<Anim::AnimationSystem>(ECS::STEP_LOCK);
	m_ecs.AddSystem<Physics::CollisionGatherSystem>(ECS::STEP_LOCK);
	m_ecs.AddSystem<Physics::ApplyImpulseSystem>(ECS::STEP_LOCK);
	m_ecs.AddSystem<Physics::MovementSystem>(ECS::STEP_LOCK);
	m_ecs.AddSystem<QuadtreeUpdateSystem>(ECS::STEP_LOCK);
	m_ecs.AddSystem<Navigation::SteeringSystem>(ECS::STEP_LOCK);

	m_ecs.AddSystem<DebugDrawSystem>(ECS::POST_STEP_LOCK);
	m_ecs.AddSystem<TransformUpdateSystem>(ECS::POST_STEP_LOCK);
	m_ecs.AddSystem<Rendering::CullingSystem>(ECS::POST_STEP_LOCK);
	m_ecs.AddSystem<Rendering::SkinningSystem>(ECS::POST_STEP_LOCK);
	m_ecs.AddSystem<ImGuiSystem>(ECS::POST_STEP_LOCK);
	m_ecs.AddSystem<Rendering::RenderSystem>(ECS::POST_STEP_LOCK);

	m_ecs.ExecuteForAllSystems([this](SystemBase* system)
	{
		system->m_ecs          = &m_ecs;
		system->m_frameCounter = &m_frameCounter;
		system->Init();
	});

	Navigation::SteeringSystem* steering = GetSystem<Navigation::SteeringSystem>();

	GetSystem<ImGuiSystem>()->m_onNewFrame.AddListener([steering]()
	{
		steering->DrawMovementTester();
	});
}
