#pragma once

#include "Entity.h"
#include "Containers/LookupList.h"
#include "Quadtree/Quadtree.h"

struct SpawnParams
{
	Point3D m_position    = Point3D();
	Quaternion m_rotation = Quaternion::identity;
	Vector3D m_scale      = Vector3D(1.0f, 1.0f, 1.0f);
};

class World
{
	friend class Entity;
	friend class Application;

public:

	World();

	template<class T, class... Ts, class = std::enable_if_t<std::is_base_of_v<Entity, T>>>
	Ref<T> Spawn(const SpawnParams spawnParams, Ts&&... val)
	{
		const uint64_t instanceId         = GenerateInstanceId();
		const std::shared_ptr<Entity> obj = std::static_pointer_cast<Entity>(std::make_shared<T>(std::forward<Ts>(val)...));

		//TODO: Use m_pendingSpawnEntities
		m_entities.Add(obj, instanceId);
		SetupSpawnedEntity(obj, instanceId, spawnParams);

		return std::dynamic_pointer_cast<T>(obj);
	}

	Ref<Entity> GetEntity(uint64_t instanceId);

	[[nodiscard]] Quadtree& GetQuadtree() { return m_quadtree; }

	static World* Get();

private:

	void Update();

	uint64_t GenerateInstanceId() const;
	void SetupSpawnedEntity(const std::shared_ptr<Entity>& entity, uint64_t instanceId, const SpawnParams& spawnParams);
	void AddToPendingDestroy(Ref<Entity> entity);

	LookupList<std::shared_ptr<Entity>, uint64_t> m_entities;

	std::unordered_set<uint64_t> m_pendingDestroyList;
	std::unordered_map<uint64_t, std::shared_ptr<Entity>> m_pendingSpawnEntities;

	Quadtree m_quadtree;
};
