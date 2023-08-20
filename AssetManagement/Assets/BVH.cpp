#include "stdafx.h"
#include "BVH.h"

#include "Mesh.h"

BVH::Node::Node(BVH& bvh, const uint32_t startIndex, const uint32_t triCount, const bool verticalSplit) :
	m_startTriangleId(startIndex),
	m_triangleCount(triCount)
{
	if (triCount == 0)
	{
		return;
	}
	
	CalculateBoundingBox(bvh);
	
	if (triCount > MAX_LEAF_TRIANGLE_COUNT)
	{
		const uint32_t splitPoint = SplitTriangles(bvh, m_boundingBox.GetCenter(), verticalSplit);

		const uint32_t leftTriCount = splitPoint - startIndex;

		m_left  = std::make_unique<Node>(bvh, startIndex, leftTriCount, !verticalSplit);
		m_right = std::make_unique<Node>(bvh, splitPoint, triCount - leftTriCount, !verticalSplit);
	}
}

void BVH::Node::CalculateBoundingBox(const BVH& bvh)
{
	{
		const Triangle triangle = bvh.GetTriangle(m_startTriangleId);
		m_boundingBox           = BoundingBox{ triangle.m_vertices[0], triangle.m_vertices[0] };
		m_boundingBox.Append(triangle.m_vertices[1]);
		m_boundingBox.Append(triangle.m_vertices[2]);
	}
	for (uint32_t i = 1; i < m_triangleCount; ++i)
	{
		const Triangle triangle = bvh.GetTriangle(m_startTriangleId + i);
		m_boundingBox.Append(triangle.m_vertices[0]);
		m_boundingBox.Append(triangle.m_vertices[1]);
		m_boundingBox.Append(triangle.m_vertices[2]);
	}
}

uint32_t BVH::Node::SplitTriangles(BVH& bvh, const Point3D& center, const bool verticalSplit) const
{
	int32_t i = m_startTriangleId;
	int32_t j = m_startTriangleId + m_triangleCount - 1;

	const uint32_t componentId = verticalSplit ? 1 : 0;

	while (i <= j)
	{
		const Triangle leftTriangle = bvh.GetTriangle(i);
		const Point3D leftCentroid  = (leftTriangle.m_vertices[0] + leftTriangle.m_vertices[1] + leftTriangle.m_vertices[2]) / 3.0f;
		if (leftCentroid[componentId] < center[componentId])
		{
			++i;
		}
		else
		{
			bvh.SwapTriangles(i, j--);
		}
	}

	return i;
}

void BVH::Node::IntersectNode(const BVH& bvh, const Ray& ray, float& distance) const
{
	if (RayIntersectionTester(ray).Intersect(m_boundingBox) < 0.0f)
	{
		return;
	}

	if (m_left == nullptr)
	{
		for (uint32_t i = 0; i < m_triangleCount; ++i)
		{
			const Triangle triangle       = bvh.GetTriangle(i + m_startTriangleId);
			const float intersectDistance = ::Intersect(ray, triangle);
			if (intersectDistance >= 0.0f && (intersectDistance < distance || distance < 0.0f))
			{
				distance = intersectDistance;
			}
		}
	}
	else
	{
		m_left->IntersectNode(bvh, ray, distance);
		m_right->IntersectNode(bvh, ray, distance);
	}
}

BVH::BVH(const std::shared_ptr<Mesh>& mesh)
{
	m_mesh = mesh;

	uint32_t triangleOffset = 0;
	m_submeshTriangleOffsets.reserve(mesh->GetSubmeshCount());
	for (const Submesh& submesh : mesh->GetSubmeshes())
	{
		m_submeshTriangleOffsets.emplace_back(triangleOffset);
		triangleOffset += submesh.GetIndexBuffer().GetElementCount() / 3;
	}

	m_triangleMap.resize(triangleOffset);
	for (uint32_t i = 0; i < triangleOffset; ++i)
	{
		m_triangleMap[i] = i;
	}

	m_root = std::make_unique<Node>(*this, 0, triangleOffset, false);
}

Triangle BVH::GetTriangle(uint32_t index) const
{
	index = m_triangleMap[index];

	uint32_t submeshId = 0;
	for (uint32_t i = 0; i < m_submeshTriangleOffsets.size(); ++i)
	{
		if (m_submeshTriangleOffsets.size() - 1 == i || m_submeshTriangleOffsets[i + 1] > index)
		{
			submeshId = i;
			index -= m_submeshTriangleOffsets[i];
			break;
		}
	}

	const MeshBuffer& indexBuffer = m_mesh->GetSubmesh(submeshId).GetIndexBuffer();
	uint32_t indices[3];
	memcpy(indices, &indexBuffer.m_data[indexBuffer.m_elementSize * index * 3], sizeof(indices));

	const MeshBuffer& vertexBuffer = m_mesh->GetSubmesh(submeshId).GetVertexBuffer();

	Triangle triangle;
	for (uint32_t i = 0; i < 3; ++i)
	{
		memcpy(&triangle.m_vertices[i], &vertexBuffer.m_data[sizeof(Point3D) * indices[i]], sizeof(Point3D));
	}

	return triangle;
}

std::optional<Point3D> BVH::Intersect(const Ray& ray) const
{
	float distance = -1.0f;
	m_root->IntersectNode(*this, ray, distance);

	if (distance >= 0.0f)
	{
		return ray.m_origin + ray.m_direction * distance;
	}
	{
		return std::optional<Point3D>();
	}
}

void BVH::SwapTriangles(const uint32_t a, const uint32_t b)
{
	std::swap(m_triangleMap[a], m_triangleMap[b]);
}
