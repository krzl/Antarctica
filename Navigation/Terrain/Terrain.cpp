#include "stdafx.h"
#include "Terrain.h"

#include "Assets/Mesh.h"
#include "Assets/Loaders/SubmeshBuilder.h"

namespace Navigation
{
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

	Point3D Terrain::GetPos(const uint32_t id) const
	{
		const float xCenterOffset = m_width * GRID_CELL_TO_METER / 2.0f;
		const float yCenterOffset = m_height * GRID_CELL_TO_METER / 2.0f;

		return Point3D((id % m_width) * GRID_CELL_TO_METER - xCenterOffset,
			(id / m_width) * GRID_CELL_TO_METER - yCenterOffset,
			TerrainHeightLevelToZ(m_heightMap[id]));
	}

	bool Terrain::IsOnSlope(const Point3D& point) const
	{
		const float xCenterOffset = m_width * GRID_CELL_TO_METER / 2.0f;
		const float yCenterOffset = m_height * GRID_CELL_TO_METER / 2.0f;

		const float x = (point.x + xCenterOffset) / GRID_CELL_TO_METER;
		const float y = (point.y + yCenterOffset) / GRID_CELL_TO_METER;

		if (x < 0 || y < 0 || x > m_width || y > m_height)
		{
			return true;
		}

		const HeightLevel a = GetHeight((uint32_t) x + 1, (uint32_t) y);
		const HeightLevel b = GetHeight((uint32_t) x, (uint32_t) y + 1);
		const HeightLevel c = (1 - Terathon::Frac(x)) > Terathon::Frac(y) ? GetHeight((uint32_t) x, (uint32_t) y) : GetHeight((uint32_t) x + 1, (uint32_t) y + 1);

		return Max(a, Max(b, c)) - Min(a, Min(b, c)) > 1;
	}

	uint32_t Terrain::GetHeightDifference(const uint32_t a, const uint32_t b, const uint32_t c) const
	{
		const HeightLevel aHeight = m_heightMap[a];
		const HeightLevel bHeight = m_heightMap[b];
		const HeightLevel cHeight = m_heightMap[c];

		const HeightLevel min = Min(aHeight, Min(bHeight, cHeight));
		const HeightLevel max = Max(aHeight, Max(bHeight, cHeight));

		return max - min;
	}

	float Terrain::TerrainHeightLevelToZ(const HeightLevel level)
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

	void Terrain::ConstructSubmesh(const std::shared_ptr<Mesh> mesh, uint32_t xStart, const uint32_t xEnd, uint32_t yStart, const uint32_t yEnd) const
	{
		uint32_t currentIndex = 0;

		const float xCenterOffset = m_width * GRID_CELL_TO_METER / 2.0f;
		const float yCenterOffset = m_height * GRID_CELL_TO_METER / 2.0f;

		const uint32_t vertexCount = (xEnd - xStart) * (yEnd - yStart);
		const uint32_t indexCount  = (xEnd - xStart - 1) * (yEnd - yStart - 1) * 6;

		std::vector<Point3D> vertices(vertexCount);
		std::vector<float> texcoordSplatUV(vertexCount * 2);
		std::vector<float> texcoordWeights(vertexCount * 4);
		std::vector<uint32_t> indices(indexCount);

		// ReSharper disable once CppInconsistentNaming
		auto GetSubmeshIndex = [xStart, xEnd, yStart](const uint32_t x, const uint32_t y)
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
					indices[currentIndex++] = a2;
					indices[currentIndex++] = b1;
					indices[currentIndex++] = b1;
					indices[currentIndex++] = a2;
					indices[currentIndex++] = b2;
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
}