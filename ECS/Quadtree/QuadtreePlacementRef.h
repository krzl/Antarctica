#pragma once

struct ColliderComponent;
class Entity;

class QuadtreePlacementRef
{
	friend class QuadtreeNode;
	friend class Quadtree;

public:

	QuadtreePlacementRef() = default;

	void Refresh(const BoundingBox& boundingBox);

	[[nodiscard]] QuadtreeNode* GetNode() const { return m_node; }

private:

	QuadtreePlacementRef(QuadtreeNode* node, Entity* object) :
		m_node(node),
		m_object(object) { }

	QuadtreeNode* m_node = nullptr;
	Entity* m_object;
};
