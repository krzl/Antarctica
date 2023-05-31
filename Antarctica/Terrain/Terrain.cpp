#include "stdafx.h"
#include "Terrain.h"

#include "Assets/Mesh.h"
#include "Loaders/SubmeshBuilder.h"

#define GRID_CELL_TO_METER (1.0f/4.0f)
#define GRID_LEVEL_HEIGHT_TO_METER 1.5f

Terrain::HeightLevel Terrain::GetHeight(const uint32_t x, const uint32_t y) const
{
	return m_heightMap[GetHeightMapArrayIndex(x, y)];
}

void Terrain::SetHeight(const uint32_t x, const uint32_t y, const HeightLevel level)
{
	if (x < m_width && y < m_height)
	{
		m_heightMap[GetHeightMapArrayIndex(x, y)] = level;
	}
}

uint32_t Terrain::GetHeightMapArrayIndex(const uint32_t x, const uint32_t y) const
{
	return x + y * m_height;
}

static float TerrainHeightLevelToZ(const Terrain::HeightLevel level)
{
	const float rampRatio = (float) (Clamp(level % 5, 1, 4) - 1) / 4.0f;
	return ((float) (level / 5) + rampRatio) * GRID_LEVEL_HEIGHT_TO_METER;
}

std::shared_ptr<Mesh> Terrain::ConstructMesh() const
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();


	std::vector<Vector3D> vertices(m_heightMap.size());
	std::vector<float>    texcoordSplatUV(m_heightMap.size() * 2);
	std::vector<float>    texcoordWeights(m_heightMap.size() * 4);
	std::vector<uint32_t> indices((m_width - 1) * (m_height - 1) * 6);

	const float xCenterOffset = m_width * GRID_CELL_TO_METER / 2.0f;
	const float yCenterOffset = m_height * GRID_CELL_TO_METER / 2.0f;

	constexpr float slopeUvScaleRatio = GRID_CELL_TO_METER / GRID_LEVEL_HEIGHT_TO_METER;

	for (uint32_t x = 0; x < m_width; ++x)
	{
		for (uint32_t y = 0; y < m_height; ++y)
		{
			const uint32_t idx = GetHeightMapArrayIndex(x, y);
		}
	}

	uint32_t currentIndex = 0;

	for (uint32_t x = 0; x < m_width; ++x)
	{
		for (uint32_t y = 0; y < m_height; ++y)
		{
			const uint32_t a1 = GetHeightMapArrayIndex(x, y);

			const bool isFlat =
				GetHeight(x, y) == GetHeight(Clamp(0u, m_width - 1, x), Clamp(0u, m_height - 1, y + 1)) &&
				GetHeight(x, y) == GetHeight(Clamp(0u, m_width - 1, x + 1), Clamp(0u, m_height - 1, y)) &&
				GetHeight(x, y) == GetHeight(Clamp(0u, m_width - 1, x + 1), Clamp(0u, m_height - 1, y + 1)) &&
				GetHeight(x, y) == GetHeight(Clamp(0u, m_width - 1, x - 1), Clamp(0u, m_height - 1, y)) &&
				GetHeight(x, y) == GetHeight(Clamp(0u, m_width - 1, x - 1), Clamp(0u, m_height - 1, y + 1)) &&
				GetHeight(x, y) == GetHeight(Clamp(0u, m_width - 1, x), Clamp(0u, m_height - 1, y - 1)) &&
				GetHeight(x, y) == GetHeight(Clamp(0u, m_width - 1, x + 1), Clamp(0u, m_height - 1, y - 1)) &&
				GetHeight(x, y) == GetHeight(Clamp(0u, m_width - 1, x - 1), Clamp(0u, m_height - 1, y - 1));

			if (x != m_width - 1 && y != m_height - 1)
			{
				const uint32_t a2 = GetHeightMapArrayIndex(x, y + 1);
				const uint32_t b1 = GetHeightMapArrayIndex(x + 1, y);
				const uint32_t b2 = GetHeightMapArrayIndex(x + 1, y + 1);

				indices[currentIndex++] = a1;
				indices[currentIndex++] = b1;
				indices[currentIndex++] = a2;
				indices[currentIndex++] = b1;
				indices[currentIndex++] = b2;
				indices[currentIndex++] = a2;

				texcoordWeights[a1 * 4 + 0] = isFlat ? 1.0f : 0.0f;
				texcoordWeights[a1 * 4 + 1] = isFlat ? 0.0f : 1.0f;
				texcoordWeights[a1 * 4 + 2] = 0.0f;
				texcoordWeights[a1 * 4 + 3] = 0.0f;
			}

			vertices[a1] = Vector3D(x * GRID_CELL_TO_METER - xCenterOffset,
									y * GRID_CELL_TO_METER - yCenterOffset,
									TerrainHeightLevelToZ(GetHeight(x, y)));

			texcoordSplatUV[a1 * 2 + 0] = (float) x / 40.0f;// * (isFlat ? 1.0f : slopeUvScaleRatio);
			texcoordSplatUV[a1 * 2 + 1] = (float) y / 40.0f;// * (isFlat ? 1.0f : slopeUvScaleRatio);
		}
	}


	//blur out transition
	for (uint32_t x = 0; x < m_width - 1; ++x)
	{
		for (uint32_t y = 0; y < m_height - 1; ++y)
		{
			const uint32_t a1 = GetHeightMapArrayIndex(x, y);

			bool isNearEdge =
				texcoordWeights[GetHeightMapArrayIndex(x, y + 1) * 4 + 1] > 0.7 ||
				texcoordWeights[GetHeightMapArrayIndex(x + 1, y) * 4 + 1] > 0.7 ||
				texcoordWeights[GetHeightMapArrayIndex(x + 1, y + 1) * 4 + 1] > 0.7;

			if (!isNearEdge && x != 0)
			{
				isNearEdge =
					texcoordWeights[GetHeightMapArrayIndex(x - 1, y) * 4 + 1] > 0.7 ||
					texcoordWeights[GetHeightMapArrayIndex(x - 1, y + 1) * 4 + 1] > 0.7;
			}
			if (!isNearEdge && y != 0)
			{
				isNearEdge =
					texcoordWeights[GetHeightMapArrayIndex(x, y - 1) * 4 + 1] > 0.7 ||
					texcoordWeights[GetHeightMapArrayIndex(x + 1, y - 1) * 4 + 1] > 0.7;
			}
			if (!isNearEdge && x != 0 && y != 0)
			{
				isNearEdge =
					texcoordWeights[GetHeightMapArrayIndex(x - 1, y - 1) * 4 + 1] > 0.7;
			}

			if (isNearEdge)
			{
				texcoordWeights[a1 * 4 + 1] = Max(texcoordWeights[a1 * 4 + 1], 0.5f);
			}
		}
	}


	SubmeshBuilder submeshBuilder("Terrain", std::move(vertices), indices);
	submeshBuilder.SetTexcoords(std::move(texcoordSplatUV), 0);
	submeshBuilder.SetTexcoords(std::move(texcoordWeights), 1);
	mesh->AddSubmesh(submeshBuilder.Build());
	return mesh;
}
