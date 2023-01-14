#include "stdafx.h"
#include "GameObject.h"

#include "World.h"

GameObject::GameObject()
{
	root = AddComponent<SceneComponent>();
}

Ref<Component> GameObject::AddComponent(const Class& clazz, const Ref<SceneComponent> parent)
{
	const std::shared_ptr<void> object = clazz.CreateObject();
	if (!object)
		return Ref<Component>();
	std::shared_ptr<Component> component = std::static_pointer_cast<Component>(object);
	if (!component)
		return Ref<Component>();

	const auto it             = m_components.insert(std::make_pair(clazz.GetId(), std::move(component)));
	it->second->m_componentId = ++m_componentCounter;

	if (const std::shared_ptr<SceneComponent> sceneComponent = std::static_pointer_cast<SceneComponent>(it->second))
	{
		sceneComponent->m_parent = parent.IsValid() ? parent : root;
	}

	if (m_self.IsValid())
	{
		it->second->Init(m_self, it->second);
	}

	return it->second;
}

void GameObject::RemoveComponent(const Ref<Component> component)
{
	if (const Component* ptr = *component)
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
	OnDestroy();
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
			OnEnabled();
			OnObjectEnabled.Dispatch(GetWorld()->GetSpawnedObject(GetInstanceId()));
		}
		else
		{
			OnDisabled();
			OnObjectDisabled.Dispatch(GetWorld()->GetSpawnedObject(GetInstanceId()));
		}
	}
}

void GameObject::InitComponents()
{
	for (auto [_, component] : m_components)
	{
		if (!component->m_self.IsValid())
		{
			component->Init(m_self, component);
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
			const auto& component = it->second;
			component->Tick();

			++it;
		}
	}

	it = m_components.begin();
	while (it != m_components.end())
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