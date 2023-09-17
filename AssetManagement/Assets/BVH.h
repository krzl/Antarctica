#pragma once

class Mesh;

class BVH
{
	struct Node
	{
		BoundingBox m_boundingBox;
		std::unique_ptr<Node> m_left;
		std::unique_ptr<Node> m_right;
		uint32_t m_startTriangleId;
		uint32_t m_triangleCount;

		static constexpr uint32_t MAX_LEAF_TRIANGLE_COUNT = 4;

		Node(BVH& bvh, uint32_t startIndex, uint32_t triCount, bool verticalSplit);

		void CalculateBoundingBox(const BVH& bvh);
		uint32_t SplitTriangles(BVH& bvh, const Point3D& center, bool verticalSplit) const;

		void IntersectNode(const BVH& bvh, const Ray& ray, float& distance) const;
	};

public:

	void Init(const std::shared_ptr<Mesh>& mesh);

	std::optional<Point3D> Intersect(const Ray& ray) const;

private:

	Triangle GetTriangle(uint32_t index) const;

	void SwapTriangles(uint32_t a, uint32_t b);

	std::shared_ptr<Mesh> m_mesh;

	std::unique_ptr<Node> m_root;

	std::vector<uint32_t> m_triangleMap;
	std::vector<uint32_t> m_submeshTriangleOffsets;
};
