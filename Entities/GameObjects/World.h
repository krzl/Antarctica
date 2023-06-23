#pragma once
#include "GameObject.h"
#include "Quadtree/Quadtree.h"

class World
{
	friend class GameObject;
	friend class Application;

public:

	World();

	template<
		class T,
		class = std::enable_if_t<std::is_base_of_v<GameObject, T>>>
	Ref<T> Spawn()
	{
		const uint64_t instanceId = GenerateInstanceId();
		std::shared_ptr<GameObject> obj = std::static_pointer_cast<GameObject>(T::GetClass().CreateObject());

		//TODO: Use m_pendingSpawnObjects
		m_gameObjectLookupMap[instanceId] = m_gameObjects.size();
		m_gameObjects.emplace_back(obj);
		SetupSpawnedGameObject(obj, instanceId);
		
		return std::dynamic_pointer_cast<T>(obj);
	}

	const std::vector<std::shared_ptr<GameObject>>& GetAllGameObjects() const
	{
		return m_gameObjects;
	}

	[[nodiscard]] Quadtree& GetQuadtree() { return m_quadtree; }

	static World* Get();

private:

	void Update(float deltaTime);

	uint64_t GenerateInstanceId();
	void     SetupSpawnedGameObject(std::shared_ptr<GameObject> gameObject, uint64_t instanceId);
	void     AddToPendingDestroy(Ref<GameObject> gameObject);

	std::unordered_map<uint64_t, std::shared_ptr<GameObject>> m_oldGameObjects;

	std::map<uint64_t, uint64_t>             m_gameObjectLookupMap;
	std::vector<std::shared_ptr<GameObject>> m_gameObjects;

	std::unordered_set<uint64_t>                              m_pendingDestroyList;
	std::unordered_map<uint64_t, std::shared_ptr<GameObject>> m_pendingSpawnObjects;

	Quadtree m_quadtree;
};
