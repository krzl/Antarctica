#pragma once

class Entity;

class QuadtreeNode
{
	friend class QuadtreePlacementRef;
	friend class Quadtree;

	enum class ChildDirection
	{
		BOT_LEFT,
		BOT_RIGHT,
		TOP_LEFT,
		TOP_RIGHT,
		COUNT
	};

	bool Contains(const Vector3D& point) const;

	bool Contains(const BoundingBox& boundingBox) const;

	QuadtreePlacementRef AddEntity(Entity* entity, const BoundingBox& boundingBox);
	void PlaceEntity(Entity* entity);
	void RemoveEntity(Entity* entity);

	QuadtreeNode* TryPushBack(Entity* entity, const BoundingBox& boundingBox);

	uint32_t UpdateEntityCount();
	
	void SetVisible(uint32_t frameId);
	void UpdateCulling(const Frustum& frustum, uint32_t frameId);

	Entity* TraceObject(const RayIntersectionTester& ray, float& minDistance) const;
	void TestIntersect(const Frustum& frustum, std::vector<Entity*>& objects) const;
	void FindNearby(const Sphere& sphere, std::function<void(Entity*)> function) const;

	void CollectChildObjects(std::vector<Entity*>& entities) const;
	void CollectChildObjects(std::function<void(Entity*)> function) const;

	[[nodiscard]] Quadtree& GetTree() const { return *m_tree; }

	QuadtreeNode* m_childNodes[(uint32_t) ChildDirection::COUNT] = { nullptr, nullptr, nullptr, nullptr };
	QuadtreeNode* m_parent                                       = nullptr;

	std::unordered_set<Entity*> m_entities;

	uint32_t m_totalObjectCount = 0;

	BoundingBox m_boundingBox;
	float m_maxHeight = 0.0f;
	float m_minHeight = 0.0f;

	std::mutex m_mutex;

	Quadtree* m_tree = nullptr;
	bool m_isDirty   = false;

public:

	[[nodiscard]] uint32_t GetLastSeenFrameId() const { return m_lastSeenFrameId; }
	[[nodiscard]] IntersectTestResult GetLastVisibilityTestResult() const { return m_lastVisibilityTestResult; }

private:

	uint32_t m_lastSeenFrameId                     = 0xFFFFFFFF;
	IntersectTestResult m_lastVisibilityTestResult = IntersectTestResult::OUTSIDE;

	void CreateChildren();
};
