#include "stdafx.h"
#include "GameState.h"

void GameState::GenerateTerrain(const Navigation::TerrainGenerator::SpawnParameters& params)
{
	m_terrain      = std::unique_ptr<Navigation::Terrain>(Navigation::TerrainGenerator::GenerateTerrain(params));
	m_navMesh      = std::unique_ptr<Navigation::NavMesh>(m_terrain->CreateNavMesh());
	m_buildingGrid = std::make_unique<BuildingGrid>(m_terrain.get());
}
