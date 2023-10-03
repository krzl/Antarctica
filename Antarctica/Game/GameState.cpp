#include "stdafx.h"
#include "GameState.h"

#include "Pathfinding/PathFinding.h"

#include "Terrain/TerrainLoader.h"

void GameState::GenerateTerrain(const Navigation::TerrainGenerator::SpawnParameters& params)
{
	m_terrain = std::unique_ptr<Navigation::Terrain>(Navigation::TerrainGenerator::GenerateTerrain(params));
	SetupTerrain();
}

void GameState::LoadTerrainFromFile(const std::string& path)
{
	m_terrain = Navigation::TerrainLoader::Load(path);
	SetupTerrain();
}

void GameState::SetupTerrain()
{
	m_navMesh      = std::unique_ptr<Navigation::NavMesh>(m_terrain->CreateNavMesh());
	m_buildingGrid = std::make_unique<BuildingGrid>(m_terrain.get());

	Navigation::PathFinding::m_navMesh = m_navMesh.get();
	Navigation::PathFinding::m_terrain = m_terrain.get();
}
