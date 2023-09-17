#include "stdafx.h"
#include "BuildStructureActivator.h"

#include "AssetManager.h"

#include "Abilities/BuildAbility.h"

#include "Assets/DynamicMesh.h"

#include "Core/Application.h"

#include "Entities/DynamicMeshEntity.h"
#include "Entities/World.h"

#include "Camera/PlayerCameraSystem.h"

#include "Game/BuildingGrid.h"
#include "Game/GameState.h"

BuildStructureActivator::BuildStructureActivator(const uint32_t width, const uint32_t height)
{
	m_width  = width;
	m_height = height;

	if (!m_placementIndicator)
	{
		std::shared_ptr<DynamicMesh> dynamicMesh = std::make_shared<DynamicMesh>();

		m_placementIndicator = World::Get()->Spawn<Rendering::DynamicMeshEntity>(SpawnParams{}, dynamicMesh);

		std::shared_ptr<Shader> shader                 = AssetManager::GetAsset<Shader>("../Resources/Shaders/build_placement.hlsl");
		std::shared_ptr<Material> material             = std::make_shared<Material>(shader);
		material->GetShaderParams().m_depthTestEnabled = true;
		material->SetOrder(3000); //TODO: create enum for sorting order

		material->GetShaderParams().m_depthTestEnabled = false;

		Rendering::MeshComponent* meshComponent = m_placementIndicator->GetComponentAccessor().GetComponent<Rendering::MeshComponent>();

		meshComponent->m_materials = { material };

		dynamicMesh->SetSubmeshCount(1);

		Submesh& submesh = dynamicMesh->GetSubmesh(0);
		submesh.SetDynamic();
		submesh.SetAttributesUsage({
			false,
			false,
			false,
			1,
			0,
			0,
			0,
			0
		});
	}
}

void BuildStructureActivator::Update()
{
	const std::optional<Point3D> cursorPosition = Application::Get().GetSystem<PlayerCameraSystem>()->GetCursorWorldPosition();

	if (!cursorPosition.has_value())
	{
		m_isGridPositionSet = false;
		m_canBePlaced       = false;

		return;
	}

	if (!m_buildingGrid)
	{
		m_buildingGrid = Application::Get().GetGameState().GetBuildingGrid();
	}

	Point2DInt centerGridPosition = m_buildingGrid->GetGridPosition(cursorPosition.value());
	centerGridPosition.x          = Clamp<uint32_t>((m_width - 1) / 2, m_buildingGrid->GetWidth() - m_width / 2 - 1, centerGridPosition.x);
	centerGridPosition.y          = Clamp<uint32_t>((m_height - 1) / 2, m_buildingGrid->GetHeight() - m_height / 2 - 1, centerGridPosition.y);

	m_startGridPosition = centerGridPosition - Point2DInt{ (int32_t) (m_width - 1) / 2, (int32_t) (m_height - 1) / 2 };
	m_endGridPosition   = centerGridPosition + Point2DInt{ (int32_t) m_width / 2, (int32_t) m_height / 2 };

	m_isGridPositionSet = true;
	m_canBePlaced       = m_buildingGrid->IsFree(m_startGridPosition, m_endGridPosition);

	const ComponentAccessor& accessor             = m_placementIndicator->GetComponentAccessor();
	const Rendering::MeshComponent* meshComponent = accessor.GetComponent<Rendering::MeshComponent>();

	const std::shared_ptr<DynamicMesh> dynamicMesh = std::static_pointer_cast<DynamicMesh>(meshComponent->m_mesh);

	Submesh& submesh = dynamicMesh->GetSubmesh(0);

	m_buildingGrid->UpdatePlacementSubmesh(submesh, m_startGridPosition, m_endGridPosition);
}

bool BuildStructureActivator::CanBeFinished()
{
	return m_canBePlaced;
}

bool BuildStructureActivator::ShouldBeCancelled()
{
	return m_buildingGrid == nullptr;
}

std::shared_ptr<Ability> BuildStructureActivator::Activate(Entity* entity)
{
	return std::make_shared<BuildAbility>(m_buildingGrid, m_startGridPosition, m_endGridPosition);
}

void BuildStructureActivator::OnFinished()
{
	m_placementIndicator->Destroy();
}
