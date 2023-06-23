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

		if (m_gameObjectLookupMap.find(candidate) == m_gameObjectLookupMap.end())
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
		m_gameObjectLookupMap[instanceId] = m_gameObjects.size();
		m_gameObjects.emplace_back(object);
	}

	m_pendingSpawnObjects.clear();

	for (auto instanceId : m_pendingDestroyList)
	{
		if (m_gameObjects.size() == 1)
		{
			m_gameObjects.clear();
			m_gameObjectLookupMap.clear();
			break;
		}

		const uint32_t                    id             = m_gameObjectLookupMap[instanceId];
		const std::shared_ptr<GameObject> objectToInsert = m_gameObjects[m_gameObjects.size() - 1];

		m_gameObjectLookupMap.erase(instanceId);
		m_gameObjectLookupMap[objectToInsert->GetInstanceId()] = id;

		m_gameObjects[id] = objectToInsert;

		m_gameObjects.erase(m_gameObjects.end() - 1);
	}

	m_pendingDestroyList.clear();

	for (const std::shared_ptr<GameObject>& objectPtr : m_gameObjects)
	{
		GameObject* gameObject = objectPtr.get();
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
		const auto it = m_gameObjectLookupMap.find(ptr->GetInstanceId());
		if (it != m_gameObjectLookupMap.end())
		{
			m_pendingDestroyList.insert(ptr->GetInstanceId());
			ptr->m_isPendingDestroy = true;

			m_gameObjects[it->second].reset();
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
