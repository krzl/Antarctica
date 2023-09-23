#pragma once

#include "BuildingGrid.h"
#include "Terrain/TerrainGenerator.h"

class GameState
{
public:

	[[nodiscard]] Navigation::Terrain* GetTerrain() const { return m_terrain.get(); }
	[[nodiscard]] Navigation::NavMesh* GetNavMesh() const { return m_navMesh.get(); }
	[[nodiscard]] BuildingGrid* GetBuildingGrid() const { return m_buildingGrid.get(); }

	void GenerateTerrain(const Navigation::TerrainGenerator::SpawnParameters& params);
	void LoadTerrainFromFile(const std::string& path);

private:

	void SetupTerrain();

	std::unique_ptr<Navigation::Terrain> m_terrain = nullptr;
	std::unique_ptr<Navigation::NavMesh> m_navMesh = nullptr;
	std::unique_ptr<BuildingGrid> m_buildingGrid   = nullptr;
};
