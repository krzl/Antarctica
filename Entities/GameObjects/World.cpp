#include "stdafx.h"
#include "World.h"

#include "GameObject.h"

static World* world = nullptr;

World::World()
{
	world = this;
}

uint64_t World::GenerateInstanceId()
{
	while (true)
	{
		const uint64_t candidate = Random::GetRandomUInt64();
		if (m_gameObjects.find(candidate) == m_gameObjects.end() && m_pendingSpawnObjects.find(candidate) ==
			m_pendingSpawnObjects.end())
		{
			return candidate;
		}
	}
}

void World::SetupSpawnedGameObject(const std::shared_ptr<GameObject> gameObject, const uint64_t instanceId)
{
	gameObject->m_self       = gameObject;
	gameObject->m_instanceId = instanceId;

	gameObject->InitComponents();

	gameObject->m_quadtreePlacement = m_quadtree.AddObject(gameObject.get(), gameObject->GetBoundingBox());
	gameObject->m_world             = this;

	if (gameObject->m_name.empty())
	{
		gameObject->SetName("GameObject Id: " + std::to_string(instanceId));
	}

	gameObject->OnCreated();
	if (gameObject->IsEnabled())
	{
		gameObject->OnEnabled();
	}
}

Ref<GameObject> World::GetSpawnedObject(const uint64_t instanceId)
{
	auto it = m_gameObjects.find(instanceId);
	if (it != m_gameObjects.end())
	{
		return Ref(it->second);
	}
	it = m_pendingSpawnObjects.find(instanceId);
	if (it != m_pendingSpawnObjects.end())
	{
		return Ref(it->second);
	}

	return Ref<GameObject>();
}

World* World::Get()
{
	return world;
}

void World::Update(float deltaTime)
{
	for (auto& [instanceId, object] : m_pendingSpawnObjects)
	{
		m_gameObjects[instanceId] = object;
	}
	m_pendingSpawnObjects.clear();

	for (auto instanceId : m_pendingDestroyList)
	{
		m_quadtree.RemoveObject(m_gameObjects[instanceId].get());
		m_gameObjects.erase(instanceId);
	}
	m_pendingDestroyList.clear();

	for (auto& [_, gameObject] : m_gameObjects)
	{
		if (!gameObject->IsPendingDestroy())
		{
			gameObject->Tick(deltaTime);
			gameObject->TickComponents(deltaTime);
		}
	}
}

void World::AddToPendingDestroy(Ref<GameObject> gameObject)
{
	if (GameObject* ptr = *gameObject)
	{
		auto it = m_gameObjects.find(ptr->GetInstanceId());
		if (it != m_gameObjects.end())
		{
			it->second.reset();

			m_pendingDestroyList.insert(ptr->GetInstanceId());
			ptr->m_isPendingDestroy = true;
		}
		else
		{
			it = m_pendingSpawnObjects.find(ptr->GetInstanceId());
			if (it != m_pendingSpawnObjects.end())
			{
				m_pendingSpawnObjects.erase(it);
			}
		}
	}
}
