#include "stdafx.h"
#include "GameObject.h"

#include "World.h"
#include "Core/Class.h"

GameObject::GameObject()
{
	OnObjectEnabled.AddListener([this](Ref<GameObject> obj)
	{
		OnEnabled();
	}, false);

	OnObjectDisabled.AddListener([this](Ref<GameObject> obj)
	{
		OnDisabled();
	}, false);

	OnObjectDestroyed.AddListener([this](Ref<GameObject> obj)
	{
		OnDestroy();
	}, false);
}

Ref<Component> GameObject::AddComponent(const Class& clazz)
{
	const std::shared_ptr<void> object = clazz.CreateObject();
	if (!object)
		return Ref<Component>();
	std::shared_ptr<Component> component = std::static_pointer_cast<Component>(object);
	if (!component)
		return Ref<Component>();

	const auto it = m_components.insert(std::make_pair(clazz.GetId(), std::move(component)));

	it->second->Init(m_self, it->second);
	return it->second;
}

void GameObject::RemoveComponent(const Ref<Component> component)
{
	if (Component* ptr = *component)
	{
		const auto [start, end] = m_components.equal_range(ptr->GetClass()->GetId());
		for (auto it = start; it == end; ++it)
		{
			if (it->second.get() == ptr)
			{
				m_components.erase(it);
				return;
			}
		}
	}
}

void GameObject::Destroy()
{
	OnObjectDestroyed.Dispatch(m_self);
	
	if (m_world)
	{
		m_world->AddToPendingDestroy(m_self);
	}
}

Ref<Component> GameObject::GetComponentFromClass(const Class& clazz)
{
	const auto [start, end] = m_components.equal_range(clazz.GetId());

	for (auto it = start; it != end; ++it)
	{
		if (it->second.get() != nullptr)
		{
			return Ref<Component>(it->second);
		}
	}
	return Ref<Component>();
}

std::vector<Ref<Component>> GameObject::GetComponentsFromClass(const Class& clazz)
{
	const auto [start, end] = m_components.equal_range(clazz.GetId());
	std::vector<Ref<Component>> components(std::distance(start, end));
	std::transform(start, end, components.begin(), [](auto& it)
	{
		return Ref<Component>(it.second);
	});

	components.erase(std::remove_if(components.begin(), components.end(), [](Ref<Component>& component)
	{
		return *component == nullptr;
	}));

	return components;
}

void GameObject::SetEnabled(const bool isEnabled)
{
	if (m_isEnabled != isEnabled)
	{
		m_isEnabled = isEnabled;
		if (m_isEnabled)
		{
			OnObjectEnabled.Dispatch(GetWorld()->GetSpawnedObject(GetInstanceId()));
		}
		else
		{
			OnObjectDisabled.Dispatch(GetWorld()->GetSpawnedObject(GetInstanceId()));
		}
	}
}

void GameObject::TickComponents()
{
	const uint32_t cachedSize = (uint32_t) m_components.size();

	auto it = m_components.begin();
	for (uint32_t i = 0; i < cachedSize; i++)
	{
		if (it->second.get() != nullptr)
		{
			auto& component = it->second;
			component->Tick();

			++it;
		}
	}

	it = m_components.begin();
	for (; it != m_components.end();)
	{
		if (it->second.get() == nullptr)
		{
			m_components.erase(it);
		}
		else
		{
			++it;
		}
	}
}
