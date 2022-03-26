#pragma once

class World
{
	friend class GameObject;
	
public:

	template<
		class T,
		class = std::enable_if_t<std::is_base_of<GameObject, T>::value>>
	Ref<T> Spawn()
	{
		auto [it, _] = m_gameObjects.emplace();
		SetupSpawnedGameObject(it->second);
		
		return Ref<T>(it->second);
	}

	Ref<GameObject> GetSpawnedObject(const uint64_t instanceId);

private:

	void Update();

	uint64_t GenerateInstanceId();
	void SetupSpawnedGameObject(std::shared_ptr<GameObject> gameObject);
	
	void AddToPendingDestroy(Ref<GameObject> gameObject);

	std::unordered_map<uint64_t, std::shared_ptr<GameObject>> m_gameObjects;
	std::unordered_set<uint64_t> m_pendingDestroyList;
};
