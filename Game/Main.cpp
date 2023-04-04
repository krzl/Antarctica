#include "stdafx.h"


#include <AssetManager.h>
#include <Core/Application.h>
#include <Entities/Camera.h>
#include <Entities/StaticMesh.h>

#include <DirectXMath.h>

#include <Animator/Animator.h>
#include <Animator/AnimatorBuilder.h>
#include <Animator/StateMachine/AnimationState.h>
#include <Animator/StateMachine/StateMachine.h>
#include <Animator/StateMachine/StateMachineBuilder.h>

#include "Entities/AnimatedMesh.h"

void main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Application app;

	app.OnApplicationInitialized.AddListener([]()
	{
		{
			const std::shared_ptr<Mesh> mesh = AssetManager::GetAsset<Mesh>("../Resources/Meshes/Northern Soul Floor Combo.fbx");

			std::shared_ptr<Anim::Animator> animator = nullptr;
			{
				using namespace Anim;

				StateMachineBuilder stateMachineBuilder;

				const std::shared_ptr<AnimationState> startState = stateMachineBuilder.AddState<
					AnimationState>(mesh->GetAnimation(0));
				const std::shared_ptr<StateMachine> stateMachine = stateMachineBuilder.Build(startState);

				AnimatorBuilder animatorBuilder(stateMachine);
				animator = animatorBuilder.Build();
			}

			Ref<Renderer::AnimatedMesh> actor = Application::Get().GetWorld().Spawn<Renderer::AnimatedMesh>();
			actor->SetPosition(Point3D(0.0, 45.5f, -10.5f));
			actor->SetRotation(90.0f, 00.0f, 0.0f);
			actor->SetScale(Vector3D(0.1f, 0.1f, 0.1f));
			actor->GetAnimatedMeshComponent()->SetMesh(mesh);
			actor->GetAnimatedMeshComponent()->SetAnimator(animator);

			const std::shared_ptr<Texture> texture = AssetManager::GetAsset<Texture>(
				"../Resources/Textures/file14.png");

			std::shared_ptr<Shader> shader   = AssetManager::GetAsset<Shader>("../Resources/Shaders/basic.hlsl");
			const auto              material = std::make_shared<Material>(shader);
			material->SetTexture("tex", texture);
			actor->GetAnimatedMeshComponent()->SetMaterial(material);

			Ref<Renderer::Camera> camera = Application::Get().GetWorld().Spawn<Renderer::Camera>();
			camera->SetPosition(Point3D(0, 0, 0));
			camera->SetRotation(0, 0, 0);
		}
	}, false);

	app.Start();
}
