#include "stdafx.h"

#include <AssetManager.h>
#include <Core/Application.h>
#include <Entities/StaticMeshEntity.h>

#include "Camera/RTSCamera.h"
#include "Characters/Character.h"

#include "Terrain/Terrain.h"

#include "Camera/PlayerCameraSystem.h"

void main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Application app;

	app.OnApplicationInitialized.AddListener([]()
	{
		{
			GameState& gameState = Application::Get().GetGameState();

			gameState.LoadTerrainFromFile("../Resources/Maps/test.bmp");

			const std::shared_ptr<Mesh> mesh = gameState.GetTerrain()->ConstructMesh();

			Ref<RTSCamera> camera = Application::Get().GetWorld().Spawn<RTSCamera>(
				{
					Point3D(0, 0, 40),
					EulerToQuaternion(20.0f, 0.0f, 0.0f)
				}
			);

			Ref<Rendering::StaticMeshEntity> terrainActor = Application::Get().GetWorld().Spawn<Rendering::StaticMeshEntity>(
				{
					Point3D(0.0, 0.0f, 0.0f),
					EulerToQuaternion(0.0f, 0.0f, 0.0f),
					Vector3D(1.0f, 1.0f, 1.0f)
				},
				mesh);

			const std::shared_ptr<Texture> ground = AssetManager::GetAsset<Texture>("../Resources/Textures/ground.png");
			const std::shared_ptr<Texture> rock   = AssetManager::GetAsset<Texture>("../Resources/Textures/rock.png");
			std::shared_ptr<Shader> terrainShader = AssetManager::GetAsset<Shader>("../Resources/Shaders/terrain.hlsl");

			const auto material = std::make_shared<Material>(terrainShader);
			material->SetTexture("tex1", ground);
			material->SetTexture("tex2", rock);
			material->SetOrder(3);
			terrainActor->SetMaterial(material);

			PlayerCameraSystem* playerCameraSystem = Application::Get().GetSystem<PlayerCameraSystem>();
			playerCameraSystem->SetupTerrainBvh(mesh);

#if defined(DEBUG) | defined(_DEBUG)
			constexpr uint32_t gridSize = 12;
#else
			constexpr uint32_t gridSize = 40;
#endif

			for (uint32_t i = 0; i < gridSize; i++)
			{
				for (uint32_t j = 0; j < gridSize; j++)
				{
					const int32_t x = i - gridSize / 2;
					const int32_t y = j - gridSize / 2;

					Ref character = Application::Get().GetWorld().Spawn<Character>(
						{ Point3D(x * 0.65f, y * 0.65, 0.0f) });
					character->SetName("Character " + std::to_string((gridSize * i) + j));

					playerCameraSystem->AddToSelection(*character);
				}
			}
		}
	});

	app.Start();
}
