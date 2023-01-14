#include "stdafx.h"


#include <AssetManager.h>
#include <Core/Application.h>
#include <Rendering/Camera.h>
#include <Rendering/StaticMesh.h>

#include <DirectXMath.h>

void main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		Application app;

		app.OnApplicationInitialized.AddListener([]()
		{
			{
				Ref<StaticMesh> staticMesh = Application::Get().GetWorld().Spawn<StaticMesh>();
				staticMesh->SetPosition(Point3D(0.0, 45.5f, -10.5f));
				staticMesh->SetRotation(90.0f, 00.0f, 0.0f);
				staticMesh->SetScale(Vector3D(0.1f, 0.1f, 0.1f));
				staticMesh->GetStaticMeshComponent()->SetMesh(AssetManager::GetAsset<Mesh>("../Resources/Meshes/Running Arc.fbx"));
				
				const std::shared_ptr<Texture> texture = AssetManager::GetAsset<Texture>("../Resources/Textures/file14.png");
				
				std::shared_ptr<Shader> shader           = AssetManager::GetAsset<Shader>("../Resources/Shaders/basic.hlsl");
				const std::shared_ptr<Material> material = std::make_shared<Material>(shader);
				material->SetTexture("tex", texture);
				staticMesh->GetStaticMeshComponent()->SetMaterial(material);

				Ref<Camera> camera = Application::Get().GetWorld().Spawn<Camera>();
				camera->SetPosition(Point3D(0, 0, 0));
				camera->SetRotation(0, 0, 0);
			}
			/*{
				Ref<StaticMesh> staticMesh = Application::Get().GetWorld().Spawn<StaticMesh>();
				staticMesh->SetPosition(Point3D(30.0, 45.5f, -10.5f));
				staticMesh->SetRotation(90.0f, 00.0f, 0.0f);
				staticMesh->SetScale(Vector3D(0.1f, 0.1f, 0.1f));
				staticMesh->GetStaticMeshComponent()->SetMesh(AssetManager::GetAsset<Mesh>("../Resources/Meshes/Running Arc.fbx"));

				const std::shared_ptr<Texture> texture = AssetManager::GetAsset<Texture>("../Resources/Textures/uv-test.png");
				
				std::shared_ptr<Shader> shader           = AssetManager::GetAsset<Shader>("../Resources/Shaders/basic.hlsl");
				const std::shared_ptr<Material> material = std::make_shared<Material>(shader);
				material->SetTexture("tex", texture);
				staticMesh->GetStaticMeshComponent()->SetMaterial(material);
			}*/
		}, false);

		app.Start();
	}
	catch (std::exception e)
	{
		throw;
	}
}
