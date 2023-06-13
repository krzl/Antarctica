#pragma once

namespace Collision
{
	enum class CollisionChannel;
}

class GameObject;

class Quadtree
{
public:

	struct Node;

	struct PlacementRef
	{
		friend struct Node;
		friend class Quadtree;

		PlacementRef() = default;

		void InvalidatePlacement();
		bool IsValid() const;

		[[nodiscard]] Node* GetNode() const { return m_node; }

	private:

		PlacementRef(Node* node, GameObject* object) :
			m_node(node),
			m_object(object) { }

		void Refresh();

	private:

		Node*       m_node = nullptr;
		GameObject* m_object;
	};

	struct Node
	{
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
		void AddObjectInner(const GameObject* object, const BoundingBox& boundingBox);

		PlacementRef AddObject(GameObject* object, const BoundingBox& boundingBox);
		Node*        TryPushBack(GameObject* object, const BoundingBox& boundingBox);
		void         RemoveObject(GameObject* object);


		void IncrementCollisionCounts(const GameObject* object);
		void DecrementCollisionCountsInner(const GameObject* object);
		void DecrementCollisionCounts(const GameObject* object);

		[[nodiscard]] Quadtree& GetTree() const { return *m_tree; }

		Node* m_childNodes[(uint32_t) ChildDirection::COUNT] = { nullptr, nullptr, nullptr, nullptr };
		Node* m_parent                                       = nullptr;

		std::unordered_set<GameObject*> m_objects;

		uint32_t m_childObjectCount[32] = { 0 };
		uint32_t m_totalObjectCount;

		BoundingBox m_boundingBox;
		float       m_maxHeight = 0.0f;
		float       m_minHeight = 0.0f;

		std::mutex m_mutex;

		Quadtree* m_tree = nullptr;

	private:

		void RemoveObjectInner(GameObject* object);
		void CreateChildren();
	};

	Quadtree();

	PlacementRef AddObject(GameObject* object, BoundingBox boundingBox);
	void         RemoveObject(GameObject* object);

	void Update();

	[[nodiscard]] Node* GetRoot() const { return m_root; }

private:

	void ExpandRoot(const Vector3D& point);

	void InvalidatePlacement(PlacementRef* placement);

	void ThreadLoop();


	Node* m_root = nullptr;

	std::mutex               m_mutex;
	std::condition_variable  m_awaitCV;
	std::condition_variable  m_updateEndCV;
	std::vector<std::thread> m_threads;

	std::vector<PlacementRef*> m_updateList;

	static constexpr uint32_t NUM_THREADS = 4;

	static constexpr float INITIAL_ROOT_SIZE = 64.0f;
};
