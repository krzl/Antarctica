#include "stdafx.h"
#include "World.h"


#include "GameObject.h"

uint64_t World::GenerateInstanceId()
{
	while (true)
	{
		const uint64_t candidate = Random::GetRandomUInt64();
		if (m_gameObjects.find(candidate) != m_gameObjects.end())
		{
			return candidate;
		}
	}
}

void World::SetupSpawnedGameObject(const std::shared_ptr<GameObject> gameObject)
{
	gameObject->m_world = this;
	gameObject->m_instanceId = GenerateInstanceId();
	gameObject->m_self = gameObject;

	gameObject->OnCreated();
	if (gameObject->IsEnabled())
	{
		gameObject->OnEnabled();
	}
}

Ref<GameObject> World::GetSpawnedObject(const uint64_t instanceId)
{
	const auto it = m_gameObjects.find(instanceId);
	if (it != m_gameObjects.end())
	{
		return Ref<GameObject>(it->second);
	}

	return Ref<GameObject>();
}

void World::Update()
{
	std::shared_ptr<GameObject> ptr;

	for (auto&[_, gameObject] : m_gameObjects)
	{
		if (!gameObject->IsPendingDestroy())
		{
			gameObject->Tick();
		}
	}

	for (auto instanceId : m_pendingDestroyList)
	{
		m_gameObjects.erase(instanceId);
	}

	m_pendingDestroyList.clear();
}

void World::AddToPendingDestroy(const Ref<GameObject> gameObject)
{
	if (GameObject* ptr = *gameObject)
	{
		const auto it = m_gameObjects.find(ptr->GetInstanceId());
		if (it != m_gameObjects.end())
		{
			it->second.reset();
		}
		m_pendingDestroyList.insert(ptr->GetInstanceId());
		ptr->m_isPendingDestroy = true;
	}
}
