#include "stdafx.h"
#include "Peasant.h"

#include "AssetManager.h"
#include "imgui.h"
#include "Animator/AnimatorBuilder.h"
#include "Animator/StateMachine/AnimationState.h"
#include "Animator/StateMachine/StateMachineBuilder.h"
#include "Core/Application.h"
#include "Entities/AnimatedMeshComponent.h"
#include "Entities/MovementComponent.h"
#include "GameObjects/World.h"
#include "Input/InputSystem.h"

enum PeasantAnimTrigger
{
	WALK,
	WORK
};

Peasant::Peasant()
{
	m_animatedMeshComponent = AddComponent<Renderer::AnimatedMeshComponent>();
	m_movementComponent     = AddComponent<Navigation::MovementComponent>();

	m_animatedMeshComponent->SetLocalScale(Vector3D(0.01f, 0.01f, 0.01f));
	m_animatedMeshComponent->SetLocalRotation(90.0, 180.0f, 0.0f);

	const std::shared_ptr<Mesh> mesh = AssetManager::GetAsset<Mesh>(
		"../Resources/Meshes/TT_RTS_Demo_Character.FBX");

	std::shared_ptr<Anim::Animator> animator;
	{
		using namespace Anim;

		StateMachineBuilder stateMachineBuilder;

		const std::shared_ptr<Animation> idleAnim = AssetManager::GetAsset<Animation>(
			"../Resources/Animations/infantry_01_idle.fbx");
		const std::shared_ptr<AnimationState> idleState = stateMachineBuilder.AddState<AnimationState>(idleAnim);

		const std::shared_ptr<Animation> walkAnim = AssetManager::GetAsset<Animation>(
			"../Resources/Animations/infantry_03_run.fbx");
		const std::shared_ptr<AnimationState> walkState = stateMachineBuilder.AddState<AnimationState>(walkAnim);


		const std::shared_ptr<Animation> workAnim = AssetManager::GetAsset<Animation>(
			"../Resources/Animations/infantry_04_attack_A.fbx");
		const std::shared_ptr<AnimationState> workState = stateMachineBuilder.AddState<AnimationState>(workAnim);

		idleState->AddTransition({
									 true,
									 0.3f,
									 { { WALK } },
									 walkState.get()
								 }, true);

		idleState->AddTransition({
									 true,
									 0.3f,
									 { { WORK } },
									 workState.get()
								 }, true);

		const std::shared_ptr<StateMachine> stateMachine = stateMachineBuilder.Build(idleState);

		AnimatorBuilder animatorBuilder(stateMachine);
		animator = animatorBuilder.Build();
	}


	m_animatedMeshComponent->SetMesh(mesh);
	m_animatedMeshComponent->SetAnimator(animator);

	const std::shared_ptr<Texture> texture = AssetManager::GetAsset<Texture>(
		"../Resources/Textures/TT_RTS_Units_blue.png");

	std::shared_ptr<Shader> shader   = AssetManager::GetAsset<Shader>("../Resources/Shaders/basic.hlsl");
	const auto              material = std::make_shared<Material>(shader);
	material->SetTexture("tex", texture);

	std::shared_ptr<Shader> shaderSkin   = AssetManager::GetAsset<Shader>("../Resources/Shaders/basic_skinned.hlsl");
	const auto              materialSkin = std::make_shared<Material>(shaderSkin);
	materialSkin->SetTexture("tex", texture);
	m_animatedMeshComponent->SetMaterial(material);
	m_animatedMeshComponent->SetMaterial(materialSkin, 3);

	m_movementComponent->SetRadius(0.3f);
}

void Peasant::OnCreated()
{
	InputSystem::GetInstance()->OnRightMouseButtonPressed.AddListener([this]()
	{
		const MousePosition pos = InputSystem::GetInstance()->GetMousePosition();

		const float ndcX = (2.0f * pos.first) / Application::Get().GetWindow().GetWidth() - 1.0f;
		const float ndcY = 1.0f - (2.0f * pos.second) / Application::Get().GetWindow().GetHeight();

		const Vector4D clipCoords = Vector4D(ndcX, ndcY, 1.0f, 1.0f);

		Matrix4D inversePerspectiveMatrix = Inverse(m_camera->GetPerspectiveMatrix());
		Vector4D eyeCoordinates           = inversePerspectiveMatrix * clipCoords;
		eyeCoordinates /= eyeCoordinates.w;

		Matrix4D inverseViewMatrix = Inverse(m_camera->GetLookAtMatrix());
		Vector4D rayWorld          = inverseViewMatrix * eyeCoordinates;

		Vector3D direction = rayWorld.xyz - m_camera->GetWorldPosition();
		direction          = direction.Normalize();

		Ray ray = { m_camera->GetWorldPosition(), direction };

		double  t = -ray.m_origin.z / ray.m_direction.z;
		Point3D intersectionPoint;
		intersectionPoint.x = ray.m_origin.x + t * ray.m_direction.x;
		intersectionPoint.y = ray.m_origin.y + t * ray.m_direction.y;
		intersectionPoint.z = ray.m_origin.z + t * ray.m_direction.z;

		m_movementComponent->MoveTo(intersectionPoint);

		m_animatedMeshComponent->SetTrigger(WALK, true);
	}, GetRef());

	m_movementComponent->m_onArrive.AddListener([this]()
	{
		m_animatedMeshComponent->SetTrigger(WALK, false);
	});
}