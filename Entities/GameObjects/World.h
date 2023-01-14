#pragma once

class World
{
	friend class GameObject;
	friend class Application;

public:

	template<
		class T,
		class = std::enable_if_t<std::is_base_of_v<GameObject, T>>>
	Ref<T> Spawn()
	{
		std::shared_ptr<GameObject> obj     = std::static_pointer_cast<GameObject>(T::GetClass().CreateObject());
		auto                        [it, _] = m_gameObjects.emplace(std::make_pair(GenerateInstanceId(), obj));
		SetupSpawnedGameObject(it->second, it->first);

		return std::dynamic_pointer_cast<T>(it->second);
	}

	Ref<GameObject> GetSpawnedObject(const uint64_t instanceId);

private:

	void Update();

	uint64_t GenerateInstanceId();
	void     SetupSpawnedGameObject(std::shared_ptr<GameObject> gameObject, uint64_t instanceId);
	void     AddToPendingDestroy(Ref<GameObject> gameObject);

	std::unordered_map<uint64_t, std::shared_ptr<GameObject>> m_gameObjects;
	std::unordered_set<uint64_t>                              m_pendingDestroyList;
	std::unordered_map<uint64_t, std::shared_ptr<GameObject>> m_pendingSpawnObjects;
};
