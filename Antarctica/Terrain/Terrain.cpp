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
	return x + y * m_width;
}

static float TerrainHeightLevelToZ(const Terrain::HeightLevel level)
{
	const float rampRatio = (float) (Clamp(level % 5, 1, 4) - 1) / 4.0f;
	return ((float) (level / 5) + rampRatio) * GRID_LEVEL_HEIGHT_TO_METER;
}

std::shared_ptr<Mesh> Terrain::ConstructMesh() const
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

	constexpr uint32_t gridSubmeshRatio = 64;

	const uint32_t xSubmeshCount = ceil((float) m_width / gridSubmeshRatio);
	for (uint32_t i = 0; i < xSubmeshCount; ++i)
	{
		const uint32_t xStart = i * gridSubmeshRatio;
		const uint32_t xEnd   = i == (xSubmeshCount - 1) ? m_width : ((i + 1) * gridSubmeshRatio + 1);

		const uint32_t ySubmeshCount = ceil((float) m_height / gridSubmeshRatio);
		for (uint32_t j = 0; j < ySubmeshCount; ++j)
		{
			const uint32_t yStart = j * gridSubmeshRatio;
			const uint32_t yEnd   = j == (ySubmeshCount - 1) ? m_height : ((j + 1) * gridSubmeshRatio + 1);

			ConstructSubmesh(mesh, xStart, xEnd, yStart, yEnd);
		}
	}

	return mesh;
}

void Terrain::ConstructSubmesh(const std::shared_ptr<Mesh> mesh, uint32_t xStart, const uint32_t xEnd, uint32_t yStart,
							   uint32_t                    yEnd) const
{
	uint32_t currentIndex = 0;

	const float xCenterOffset = m_width * GRID_CELL_TO_METER / 2.0f;
	const float yCenterOffset = m_height * GRID_CELL_TO_METER / 2.0f;

	const uint32_t vertexCount = (xEnd - xStart + 1) * (yEnd - yStart + 1);
	const uint32_t indexCount  = (xEnd - xStart) * (yEnd - yStart);

	std::vector<Vector3D> vertices(vertexCount);
	std::vector<float>    texcoordSplatUV(vertexCount * 2);
	std::vector<float>    texcoordWeights(vertexCount * 4);
	std::vector<uint32_t> indices(indexCount * 6);

	// ReSharper disable once CppInconsistentNaming
	auto GetSubmeshIndex = [xStart, xEnd, yStart, this](const uint32_t x, const uint32_t y)
	{
		return (x - xStart) + (y - yStart) * (xEnd - xStart);
	};

	for (uint32_t x = xStart; x < xEnd; ++x)
	{
		for (uint32_t y = yStart; y < yEnd; ++y)
		{
			const uint32_t a1 = GetSubmeshIndex(x, y);

			const bool isFlat =
				GetHeight(x, y) == GetHeight(Clamp(0u, m_width - 1, x), Clamp(0u, m_height - 1, y + 1)) &&
				GetHeight(x, y) == GetHeight(Clamp(0u, m_width - 1, x + 1), Clamp(0u, m_height - 1, y)) &&
				GetHeight(x, y) == GetHeight(Clamp(0u, m_width - 1, x + 1), Clamp(0u, m_height - 1, y + 1)) &&
				GetHeight(x, y) == GetHeight(Clamp(0u, m_width - 1, x - 1), Clamp(0u, m_height - 1, y)) &&
				GetHeight(x, y) == GetHeight(Clamp(0u, m_width - 1, x - 1), Clamp(0u, m_height - 1, y + 1)) &&
				GetHeight(x, y) == GetHeight(Clamp(0u, m_width - 1, x), Clamp(0u, m_height - 1, y - 1)) &&
				GetHeight(x, y) == GetHeight(Clamp(0u, m_width - 1, x + 1), Clamp(0u, m_height - 1, y - 1)) &&
				GetHeight(x, y) == GetHeight(Clamp(0u, m_width - 1, x - 1), Clamp(0u, m_height - 1, y - 1));

			texcoordWeights[a1 * 4 + 0] = isFlat ? 1.0f : 0.0f;
			texcoordWeights[a1 * 4 + 1] = isFlat ? 0.0f : 1.0f;
			texcoordWeights[a1 * 4 + 2] = 0.0f;
			texcoordWeights[a1 * 4 + 3] = 0.0f;
			
			if (x != xEnd - 1 && y != yEnd - 1)
			{
				const uint32_t a2 = GetSubmeshIndex(x, y + 1);
				const uint32_t b1 = GetSubmeshIndex(x + 1, y);
				const uint32_t b2 = GetSubmeshIndex(x + 1, y + 1);

				indices[currentIndex++] = a1;
				indices[currentIndex++] = b1;
				indices[currentIndex++] = a2;
				indices[currentIndex++] = b1;
				indices[currentIndex++] = b2;
				indices[currentIndex++] = a2;
			}

			vertices[a1] = Vector3D(x * GRID_CELL_TO_METER - xCenterOffset,
									y * GRID_CELL_TO_METER - yCenterOffset,
									TerrainHeightLevelToZ(GetHeight(x, y)));

			texcoordSplatUV[a1 * 2 + 0] = (float) x / 40.0f;
			texcoordSplatUV[a1 * 2 + 1] = (float) y / 40.0f;
		}
	}

	//blur out transition
	for (uint32_t x = xStart; x < xEnd - 1; ++x)
	{
		for (uint32_t y = yStart; y < yEnd - 1; ++y)
		{
			const uint32_t a1 = GetSubmeshIndex(x, y);

			bool isNearEdge =
				texcoordWeights[(GetSubmeshIndex(x, y + 1)) * 4 + 1] > 0.7 ||
				texcoordWeights[(GetSubmeshIndex(x + 1, y)) * 4 + 1] > 0.7 ||
				texcoordWeights[(GetSubmeshIndex(x + 1, y + 1)) * 4 + 1] > 0.7;

			if (!isNearEdge && x != xStart)
			{
				isNearEdge =
					texcoordWeights[(GetSubmeshIndex(x - 1, y)) * 4 + 1] > 0.7 ||
					texcoordWeights[(GetSubmeshIndex(x - 1, y + 1)) * 4 + 1] > 0.7;
			}
			if (!isNearEdge && y != yStart)
			{
				isNearEdge =
					texcoordWeights[(GetSubmeshIndex(x, y - 1)) * 4 + 1] > 0.7 ||
					texcoordWeights[(GetSubmeshIndex(x + 1, y - 1)) * 4 + 1] > 0.7;
			}
			if (!isNearEdge && x != xStart && y != yStart)
			{
				isNearEdge =
					texcoordWeights[(GetSubmeshIndex(x - 1, y - 1)) * 4 + 1] > 0.7;
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
}
