#include "stdafx.h"


#include <AssetManager.h>
#include <Core/Application.h>
#include <Entities/StaticMesh.h>

#include "Camera/RTSCamera.h"
#include "Characters/Peasant.h"
#include "Debug/DebugDrawSystem.h"
#include "Debug/QuadtreeTester.h"
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

			Ref<Renderer::StaticMesh> actor = Application::Get().GetWorld().Spawn<Renderer::StaticMesh>();
			actor->SetPosition(Point3D(0.0, 0.0f, 0.0f));
			actor->SetRotation(0.0f, 0.0f, 0.0f);
			actor->SetScale(Vector3D(1.0f, 1.0f, 1.0f));
			actor->GetStaticMeshComponent()->SetMesh(mesh);

#if defined(DEBUG) | defined(_DEBUG)
			constexpr uint32_t gridSize = 10;
#else
			constexpr uint32_t gridSize = 100;
#endif
			for (uint32_t i = 0; i < gridSize; i++)
			{
				for (uint32_t j = 0; j < gridSize; j++)
				{
					const int32_t x = i - gridSize / 2;
					const int32_t y = j - gridSize / 2;

					Ref peasant = Application::Get().GetWorld().Spawn<Peasant>();
					peasant->SetPosition(Point3D(x / 2.0f, y / 2.0f, 0.0f));
					peasant->SetName("Peasant " + std::to_string((gridSize * i) + j));
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

			Vector3D up(1.0f, 1.0f, 1.0f);
			up.Normalize();

			DebugDrawSystem::GetInstance()->DrawBox(Point3D(15.0f, 5.0f, 5.0f), DirectionToQuaternion(up), 3.0f, 0.5f, 10.0f, 20.0f);
			DebugDrawSystem::GetInstance()->DrawSphere(Point3D(-5.0f, 5.0f, 5.0f), 2.0f, 150000.0f, Color::blue);

			Ref<RTSCamera> camera = Application::Get().GetWorld().Spawn<RTSCamera>();
			camera->SetPosition(Point3D(0, 0, 20));
			camera->SetRotation(20.0f, 0.0f, 0.0f);

			//Ref<QuadtreeTester> qtt = Application::Get().GetWorld().Spawn<QuadtreeTester>();
		}
	});

	app.Start();
}
