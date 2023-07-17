#include "stdafx.h"
#include "World.h"

#include "GameObject.h"

static World* world = nullptr;

World::World()
	: m_gameObjects([](const std::shared_ptr<GameObject>& ptr) { return ptr->GetInstanceId(); }),
	  m_tickableGameObjects([](GameObject* const&         ptr) { return ptr->GetInstanceId(); })
{
	world = this;
}

uint64_t World::GenerateInstanceId() const
{
	while (true)
	{
		const uint64_t candidate = Random::GetRandomUInt64();

		if (!m_gameObjects.Contains(candidate))
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

	gameObject->m_world = this;

	if (gameObject->m_name.empty())
	{
		gameObject->SetName("GameObject Id: " + std::to_string(instanceId));
	}


	gameObject->OnCreated();
	if (gameObject->IsEnabled())
	{
		gameObject->OnEnabled();
	}

	gameObject->m_quadtreePlacement = m_quadtree.AddObject(gameObject.get(), gameObject->GetBoundingBox());
}

World* World::Get()
{
	return world;
}

void World::Update(const float deltaTime)
{
	for (auto& [instanceId, object] : m_pendingSpawnObjects)
	{
		m_gameObjects.Add(object, instanceId);
		if (object->CanTick())
		{
			m_tickableGameObjects.Add(object.get(), instanceId);
		}
	}

	m_pendingSpawnObjects.clear();

	for (const auto instanceId : m_pendingDestroyList)
	{
		m_gameObjects.Remove(instanceId);
		if (m_tickableGameObjects.Contains(instanceId))
		{
			m_tickableGameObjects.Remove(instanceId);
		}
	}

	m_pendingDestroyList.clear();

	for (GameObject* gameObject : m_tickableGameObjects.GetItems())
	{
		if (gameObject)
		{
			gameObject->TickInner(deltaTime);
			gameObject->TickComponents(deltaTime);
		}
	}
}

void World::AddToPendingDestroy(Ref<GameObject> gameObject)
{
	if (GameObject* ptr = *gameObject)
	{
		if (std::shared_ptr<GameObject>* sharedPtr = m_gameObjects.Find(ptr->GetInstanceId()))
		{
			m_pendingDestroyList.insert(ptr->GetInstanceId());
			ptr->m_isPendingDestroy = true;

			sharedPtr->reset();
		}
		else
		{
			const auto it2 = m_pendingSpawnObjects.find(ptr->GetInstanceId());
			if (it2 != m_pendingSpawnObjects.end())
			{
				m_pendingSpawnObjects.erase(it2);
			}
		}

		m_quadtree.RemoveObject(ptr);
	}
}
