#pragma once

#include "QuadtreePlacementRef.h"

struct ColliderComponent;
class Entity;
class QuadtreeNode;

class Quadtree
{
public:

	Quadtree();

	QuadtreePlacementRef AddEntity(Entity* entity, BoundingBox boundingBox);
	void RemoveObject(Entity* entity);

	[[nodiscard]] QuadtreeNode* GetRoot() const { return m_root; }

	void UpdateEntityCounts();
	void CalculateCulling(const Frustum& frustum, uint32_t frameId);

	struct TraceResult
	{
		Ref<Entity> m_object;
		Point3D m_tracePoint;
		float m_distance;
	};

	TraceResult TraceObject(const RayIntersectionTester& ray) const;
	std::vector<Entity*> Intersect(const Frustum& frustum) const;
	void FindNearby(const Sphere& sphere, std::vector<Entity*>& outEntities) const;
	std::vector<Entity*> FindNearby(const Sphere& sphere) const;
	void FindNearby(const Sphere& sphere, std::function<void(Entity*)> function) const;

	static constexpr float QUADTREE_SIZE = 512.0f;
	static constexpr float MIN_NODE_SIZE = 4.0f;

private:

	QuadtreeNode* m_root = nullptr;
};
