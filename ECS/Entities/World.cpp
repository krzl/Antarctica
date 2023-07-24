#include "stdafx.h"
#include "World.h"

#include "Components/ColliderComponent.h"
#include "Components/TransformComponent.h"

static World* world = nullptr;

World::World() :
	m_entities([](const std::shared_ptr<Entity>& ptr) { return ptr->GetInstanceId(); })
{
	world = this;
}

Ref<Entity> World::GetEntity(const uint64_t instanceId)
{
	std::shared_ptr<Entity>* entity = m_entities.Find(instanceId);
	return entity != nullptr ? *entity : nullptr;
}

uint64_t World::GenerateInstanceId() const
{
	//TODO: make it non-rng
	while (true)
	{
		const uint64_t candidate = Random::GetRandomUInt64();

		if (!m_entities.Contains(candidate))
		{
			return candidate;
		}
	}
}

void World::SetupSpawnedEntity(const std::shared_ptr<Entity>& entity,
	const uint64_t                                            instanceId,
	const SpawnParams&                                        spawnParams)
{
	entity->m_self       = entity;
	entity->m_instanceId = instanceId;

	entity->m_world = this;

	entity->CreateArchetype();

	entity->SetName("Entity Id: " + std::to_string(instanceId));

	ComponentAccessor accessor = entity->GetComponentAccessor();
	TransformComponent* transform = accessor.GetComponent<TransformComponent>();
	if (transform)
	{
		transform->m_localPosition = spawnParams.m_position;
		transform->m_localRotation = spawnParams.m_rotation;
		transform->m_localScale    = spawnParams.m_scale;

	}

	entity->SetupComponents(accessor);

	if (transform)
	{
		if (ColliderComponent* collider = accessor.GetComponent<ColliderComponent>())
		{
			collider->m_transformedBoundingBox = collider->m_boundingBox.Transform(transform->GetWorldTransform());
		}
		transform->m_quadtreePlacement = m_quadtree.AddEntity(entity.get(), entity->GetBoundingBox());
	}
}

World* World::Get()
{
	//TODO: support multiple worlds
	return world;
}

void World::Update()
{
	for (auto& [instanceId, object] : m_pendingSpawnEntities)
	{
		m_entities.Add(object, instanceId);
	}

	m_pendingSpawnEntities.clear();

	for (const auto instanceId : m_pendingDestroyList)
	{
		m_entities.Remove(instanceId);
	}

	m_pendingDestroyList.clear();
}

void World::AddToPendingDestroy(Ref<Entity> entity)
{
	if (Entity* ptr = *entity)
	{
		if (std::shared_ptr<Entity>* sharedPtr = m_entities.Find(ptr->GetInstanceId()))
		{
			m_pendingDestroyList.insert(ptr->GetInstanceId());
		}
		else
		{
			const auto it2 = m_pendingSpawnEntities.find(ptr->GetInstanceId());
			if (it2 != m_pendingSpawnEntities.end())
			{
				m_pendingSpawnEntities.erase(it2);
			}
		}
		//TODO: only for moveable components
		m_quadtree.RemoveObject(ptr);
	}
}
