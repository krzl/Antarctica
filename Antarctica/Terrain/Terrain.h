#pragma once

class Mesh;

class Terrain
{
	friend class TerrainGenerator;

public:

	typedef int16_t HeightLevel;

	HeightLevel GetHeight(uint32_t x, uint32_t y) const;


	std::shared_ptr<Mesh> ConstructMesh() const;

private:

	Terrain() = default;

	void     SetHeight(uint32_t x, uint32_t y, HeightLevel level);

	uint32_t GetHeightMapArrayIndex(uint32_t x, uint32_t y) const;

	uint32_t                 m_width  = 0;
	uint32_t                 m_height = 0;
	std::vector<HeightLevel> m_heightMap;
};
