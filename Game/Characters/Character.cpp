#include "stdafx.h"
#include "Character.h"

#include "AssetManager.h"
#include "Abilities/AbilityBinding.h"
#include "Abilities/AbilityStackComponent.h"
#include "Abilities/AbilityTriggerComponent.h"
#include "Abilities/SelectableComponent.h"
#include "Abilities/Activators/BuildStructureActivator.h"
#include "Abilities/Activators/InstantMoveActivator.h"
#include "Animator/AnimatorBuilder.h"
#include "Animator/StateMachine/AnimationState.h"
#include "Animator/StateMachine/StateMachineBuilder.h"
#include "Archetypes/ArchetypeBuilder.h"
#include "Assets/Material.h"
#include "Assets/Shader.h"
#include "Assets/Texture.h"
#include "Components/AnimatedMeshComponent.h"
#include "Components/ColliderComponent.h"
#include "Components/MeshComponent.h"
#include "Components/MoveableComponent.h"
#include "Components/MovementComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/RenderComponent.h"
#include "Components/RenderCullComponent.h"
#include "Components/TransformComponent.h"

void Character::DefineArchetype(ArchetypeBuilder& builder)
{
	Entity::DefineArchetype(builder);

	builder.AddComponent<AbilityStackComponent>();
	builder.AddComponent<AbilityTriggerComponent>();
	builder.AddComponent<Anim::AnimatedMeshComponent>();
	builder.AddComponent<ColliderComponent>();
	builder.AddComponent<Rendering::MeshComponent>();
	builder.AddComponent<MoveableComponent>();
	builder.AddComponent<Navigation::MovementComponent>();
	builder.AddComponent<Rendering::RenderComponent>();
	builder.AddComponent<Rendering::RenderCullComponent>();
	builder.AddComponent<TransformComponent>();
	builder.AddComponent<SelectableComponent>();
	builder.AddComponent<Physics::PhysicsBodyComponent>();
}

void Character::SetupComponents(const ComponentAccessor& accessor)
{
	Entity::SetupComponents(accessor);

	AbilityTriggerComponent* abilityTrigger    = accessor.GetComponent<AbilityTriggerComponent>();
	Anim::AnimatedMeshComponent* animatedMesh  = accessor.GetComponent<Anim::AnimatedMeshComponent>();
	ColliderComponent* collider                = accessor.GetComponent<ColliderComponent>();
	Rendering::MeshComponent* meshComponent    = accessor.GetComponent<Rendering::MeshComponent>();
	Physics::PhysicsBodyComponent* physicsBody = accessor.GetComponent<Physics::PhysicsBodyComponent>();
	Navigation::MovementComponent* movement    = accessor.GetComponent<Navigation::MovementComponent>();

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
				walkState.get(),
				0.3f,
				true,
				{ { (uint32_t) AnimTrigger::WALK } },
			},
			true);

		idleState->AddTransition({
				workState.get(),
				0.3f,
				true,
				{ { (uint32_t) AnimTrigger::WORK } },
			},
			true);

		const std::shared_ptr<StateMachine> stateMachine = stateMachineBuilder.Build(idleState);

		AnimatorBuilder animatorBuilder(stateMachine);
		animator = animatorBuilder.Build();
	}

	const std::shared_ptr<Mesh> mesh = AssetManager::GetAsset<Mesh>(
		"../Resources/Meshes/TT_RTS_Demo_Character.FBX");

	const std::shared_ptr<Mesh> box = AssetManager::GetAsset<Mesh>(
		"../Resources/Meshes/Box.FBX");

	movement->m_radius = 0.23f;
	movement->m_colliderRadius = 0.18f;

	meshComponent->m_renderItems.resize(2);
	
	Rendering::RenderItem& characterRenderItem = meshComponent->m_renderItems[0];
	Rendering::RenderItem& selectionRenderItem = meshComponent->m_renderItems[1];

	characterRenderItem.m_transform = EulerToQuaternion(90.0, 180.0f, 0.0f).GetRotationMatrix() * Transform4D::MakeScale(0.01f);
	characterRenderItem.m_mesh = mesh;
	characterRenderItem.m_isAnimated = true;

	selectionRenderItem.m_transform = Transform4D::MakeScale(movement->m_radius * 0.02f);
	selectionRenderItem.m_mesh = box;
	selectionRenderItem.m_isHidden = true;

	animatedMesh->m_animator = animator;
	animatedMesh->m_animationSolver.ResetSolver(animator);

	const std::shared_ptr<Texture> texture = AssetManager::GetAsset<Texture>("../Resources/Textures/TT_RTS_Units_blue.png");

	std::shared_ptr<Shader> shader = AssetManager::GetAsset<Shader>("../Resources/Shaders/basic.hlsl");
	const auto material            = std::make_shared<Material>(shader);
	material->SetTexture("tex", texture);
	material->GetShaderParams().m_stencilMode = ShaderParams::WRITE_REPLACE;
	material->GetShaderParams().m_stencilMask = 1;
	material->GetShaderParams().m_stencilRef = 1;
	material->SetOrder(CHARACTER);

	std::shared_ptr<Shader> shaderSkin = AssetManager::GetAsset<Shader>("../Resources/Shaders/basic_skinned.hlsl");
	const auto materialSkin            = std::make_shared<Material>(shaderSkin);
	materialSkin->SetTexture("tex", texture);
	materialSkin->GetShaderParams().m_stencilMode = ShaderParams::WRITE_REPLACE;
	materialSkin->GetShaderParams().m_stencilMask = 1;
	materialSkin->GetShaderParams().m_stencilRef = 1;
	materialSkin->SetOrder(CHARACTER);

	std::shared_ptr<Shader> selectionShader = AssetManager::GetAsset<Shader>("../Resources/Shaders/decal.hlsl");
	const auto selectionMaterial = std::make_shared<Material>(selectionShader);
	selectionMaterial->SetOrder(TRANSPARENCY);
	selectionMaterial->GetShaderParams().m_blendingEnabled = true;
	selectionMaterial->GetShaderParams().m_stencilMode = ShaderParams::TEST;
	selectionMaterial->GetShaderParams().m_stencilMask = 1;
	selectionMaterial->GetShaderParams().m_stencilRef = 0;
	selectionMaterial->GetShaderParams().m_depthWriteDisabled = true;

	characterRenderItem.m_materials = { material, material, material, materialSkin };
	selectionRenderItem.m_materials = { selectionMaterial };

	collider->m_boundingBox = mesh->GetBoundingBox().Transform(characterRenderItem.m_transform);

	abilityTrigger->m_abilityBindings.emplace_back(AbilityBinding{
		"MoveAbility",
		InputCommand::Type::MOUSE_PRESS,
		(uint32_t) InputCommand::MouseButtonId::RIGHT,
		[]()
		{
			return std::make_shared<InstantMoveActivator>();
		}
	});

	abilityTrigger->m_abilityBindings.emplace_back(AbilityBinding{
		"BuildObstacle",
		InputCommand::Type::KEY_PRESS,
		(uint32_t) Key::B,
		[]()
		{
			return std::make_shared<BuildStructureActivator>(3, 3);
		}
	});

	physicsBody->SetMass(2000.0f);
}
