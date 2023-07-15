#include "stdafx.h"


#include <AssetManager.h>
#include <Core/Application.h>
#include <Entities/StaticMesh.h>

#include "Camera/RTSCamera.h"
#include "Characters/Peasant.h"
#include "Debug/MovementTester.h"
#include "Debug/QuadtreeTester.h"
#include "Debug/DebugDrawSystem.h"
#include "Entities/AnimatedMesh.h"
#include "Terrain/Terrain.h"
#include "Terrain/TerrainGenerator.h"

void main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Application app;

	app.OnApplicationInitialized.AddListener([]()
	{
		{
			constexpr TerrainGenerator::SpawnParameters params;
			const std::shared_ptr<Terrain>              terrain = TerrainGenerator::GenerateTerrain(params);
			const std::shared_ptr<Mesh>                 mesh    = terrain->ConstructMesh();

			Ref<RTSCamera> camera = Application::Get().GetWorld().Spawn<RTSCamera>();
			camera->SetPosition(Point3D(0, 0, 20));
			camera->SetRotation(20.0f, 0.0f, 0.0f);

			Ref<Renderer::StaticMesh> actor = Application::Get().GetWorld().Spawn<Renderer::StaticMesh>();
			actor->SetPosition(Point3D(0.0, 0.0f, 0.0f));
			actor->SetRotation(0.0f, 0.0f, 0.0f);
			actor->SetScale(Vector3D(1.0f, 1.0f, 1.0f));
			actor->GetStaticMeshComponent()->SetMesh(mesh);

			std::vector<Navigation::MovementComponent*> movementComponents;

#if defined(DEBUG) | defined(_DEBUG)
			constexpr uint32_t gridSize = 4;
#else
			constexpr uint32_t gridSize = 8;
#endif
			for (uint32_t i = 0; i < gridSize; i++)
			{
				for (uint32_t j = 0; j < gridSize; j++)
				{
					const int32_t x = i - gridSize / 2;
					const int32_t y = j - gridSize / 2;

					Ref peasant       = Application::Get().GetWorld().Spawn<Peasant>();
					peasant->m_camera = *camera->GetCameraComponent();
					peasant->SetPosition(Point3D(x * 0.65f, y * 0.65, 0.0f));
					peasant->SetName("Peasant " + std::to_string((gridSize * i) + j));

					movementComponents.emplace_back(peasant->GetMovementComponent());
				}
			}

			const std::shared_ptr<Texture> ground = AssetManager::GetAsset<Texture>(
				"../Resources/Textures/ground.png");

			const std::shared_ptr<Texture> rock = AssetManager::GetAsset<Texture>(
				"../Resources/Textures/rock.png");

			std::shared_ptr<Shader> terrainShader = AssetManager::GetAsset<Shader>("../Resources/Shaders/terrain.hlsl");
			const auto              material      = std::make_shared<Material>(terrainShader);
			material->SetTexture("tex1", ground);
			material->SetTexture("tex2", rock);
			material->SetOrder(3);
			actor->GetStaticMeshComponent()->SetMaterial(material);

			Ref<Navigation::MovementTester> movementTester = Application::Get().GetWorld().Spawn<Navigation::MovementTester>();
			movementTester->SetComponents(movementComponents);

			//Ref<QuadtreeTester> qtt = Application::Get().GetWorld().Spawn<QuadtreeTester>();
		}
	});

	app.Start();
}
