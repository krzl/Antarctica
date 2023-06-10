#include "stdafx.h"
#include "Peasant.h"

#include "AssetManager.h"
#include "Animator/AnimatorBuilder.h"
#include "Animator/StateMachine/AnimationState.h"
#include "Animator/StateMachine/StateMachineBuilder.h"
#include "Entities/AnimatedMeshComponent.h"
#include "Input/InputSystem.h"
#include "Movement/MovementComponent.h"

enum PeasantAnimTrigger
{
	WALK,
	WORK
};

Peasant::Peasant()
{
	m_animatedMeshComponent = AddComponent<Renderer::AnimatedMeshComponent>();
	m_movementComponent     = AddComponent<MovementComponent>();
	
	m_animatedMeshComponent->SetLocalScale(Vector3D(0.01f, 0.01f, 0.01f));
	m_animatedMeshComponent->SetLocalRotation(90.0, 0.0f, 0.0f);

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

	std::shared_ptr<Shader> shaderSkin = AssetManager::GetAsset<Shader>("../Resources/Shaders/basic_skinned.hlsl");
	const auto              materialSkin = std::make_shared<Material>(shaderSkin);
	materialSkin->SetTexture("tex", texture);
	m_animatedMeshComponent->SetMaterial(material);
	m_animatedMeshComponent->SetMaterial(material, 1);
	m_animatedMeshComponent->SetMaterial(material, 2);
	m_animatedMeshComponent->SetMaterial(materialSkin, 3);

	InputSystem::GetInstance()->OnLeftMouseButtonPressed.AddListener([this]()
	{
		using namespace Anim;

		m_isWalking = !m_isWalking;
		
		m_animatedMeshComponent->SetTrigger(WALK, m_isWalking);
	}, false);
}
