#include "stdafx.h"


#include <AssetManager.h>
#include <Core/Application.h>
#include <Entities/Camera.h>
#include <Entities/StaticMesh.h>

#include "Camera/RTSCamera.h"
#include "Characters/Peasant.h"
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
			const std::shared_ptr<Terrain>          terrain = TerrainGenerator::GenerateTerrain(params);
			const std::shared_ptr<Mesh>             mesh    = terrain->ConstructMesh();

			Ref<Renderer::StaticMesh> actor = Application::Get().GetWorld().Spawn<Renderer::StaticMesh>();
			actor->SetPosition(Point3D(0.0, 0.0f, 0.0f));
			actor->SetRotation(0.0f, 0.0f, 0.0f);
			actor->SetScale(Vector3D(1.0f, 1.0f, 1.0f));
			actor->GetStaticMeshComponent()->SetMesh(mesh);

			Ref peasant = Application::Get().GetWorld().Spawn<Peasant>();
			peasant->SetPosition(Point3D(-0.0, 0.0f, 2.0f));
			peasant->SetScale(Vector3D(0.01f, 0.01f, 0.01f));
			//peasant->SetRotation(90.0, 0.0f, 0.0f);
			
			const std::shared_ptr<Texture> ground = AssetManager::GetAsset<Texture>(
				"../Resources/Textures/ground.png");

			const std::shared_ptr<Texture> rock = AssetManager::GetAsset<Texture>(
				"../Resources/Textures/rock.png");

			std::shared_ptr<Shader> terrainShader   = AssetManager::GetAsset<Shader>("../Resources/Shaders/terrain.hlsl");
			const auto              material = std::make_shared<Material>(terrainShader);
			material->SetTexture("tex1", ground);
			material->SetTexture("tex2", rock);
			actor->GetStaticMeshComponent()->SetMaterial(material);

			
			Ref<RTSCamera> camera = Application::Get().GetWorld().Spawn<RTSCamera>();
			camera->SetPosition(Point3D(0, 0, -10));
			camera->SetRotation(70.0f, 0.0f, 0.0f);
			camera->GetCameraComponent()->SetFOV(10.0f);
		}
	}, false);

	app.Start();
}
