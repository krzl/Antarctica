#include "stdafx.h"
#include "GameObject.h"

#include "World.h"

GameObject::GameObject()
{
	m_root = AddComponent<SceneComponent>();
}

void GameObject::UpdateTickableComponents()
{
	m_tickableComponents.clear();

	for (std::shared_ptr<Component>& component : m_components)
	{
		Component* comp = component.get();
		if (comp->m_isTickable)
		{
			m_tickableComponents.push_back(comp);
		}
	}
}

Ref<Component> GameObject::AddComponent(const Class& clazz, const Ref<SceneComponent> parent)
{
	const std::shared_ptr<void> object = clazz.CreateObject();
	if (!object)
		return Ref<Component>(nullptr);
	std::shared_ptr<Component> component = std::static_pointer_cast<Component>(object);
	if (!component)
		return Ref<Component>(nullptr);

	m_components.emplace_back(component);
	component->m_componentId = ++m_componentCounter;

	if (const std::shared_ptr<SceneComponent> sceneComponent = std::dynamic_pointer_cast<SceneComponent>(component))
	{
		sceneComponent->SetParentInternal(parent.IsValid() ? parent : m_root, Ref(component).Cast<SceneComponent>());
	}

	if (GetRef().IsValid())
	{
		component->Init(GetRef(), component);
	}

	UpdateTickableComponents();

	return component;
}

void GameObject::RemoveComponent(const Ref<Component> component)
{
	if (const Component* ptr = *component)
	{
		for (auto it = m_components.begin(); it != m_components.end(); ++it)
		{
			if ((*it)->m_componentId == ptr->m_componentId)
			{
				m_components.erase(it);
				break;
			}
		}

		for (uint32_t i = 0; i < m_tickableComponents.size(); ++i)
		{
			if (m_tickableComponents[i] == ptr)
			{
				m_tickableComponents[i] = nullptr;
				break;
			}
		}
	}
}

void GameObject::Destroy()
{
	OnDestroy();
	OnObjectDestroyed.Dispatch(GetRef());

	if (World::Get())
	{
		World::Get()->AddToPendingDestroy(GetRef());
	}
}

Ref<Component> GameObject::GetComponentFromClass(const Class& clazz)
{
	for (auto it = m_components.begin(); it != m_components.end(); ++it)
	{
		if ((*it)->GetClass()->GetId() == clazz.GetId())
		{
			return Ref(*it);
		}
	}
	return Ref<Component>();
}

std::vector<Ref<Component>> GameObject::GetComponentsFromClass(const Class& clazz)
{
	std::vector<Ref<Component>> components;

	std::transform(m_components.begin(), m_components.end(), components.begin(),
				   [clazz](const std::shared_ptr<Component>& component)
				   {
					   if (component->GetClass()->GetId() == clazz.GetId())
					   {
						   return Ref(component);
					   }
					   return Ref<Component>();
				   });

	components.erase(std::remove_if(components.begin(), components.end(), [](Ref<Component>& component)
	{
		return !component.IsValid();
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
			OnObjectEnabled.Dispatch(GetRef());
		}
		else
		{
			OnDisabled();
			OnObjectDisabled.Dispatch(GetRef());
		}
	}
}

BoundingBox GameObject::GetBoundingBox() const
{
	if (m_boundingBox.has_value())
	{
		return m_boundingBox.value();
	}

	m_boundingBox = CalculateBoundingBox();
	
	return m_boundingBox.value();
}

bool GameObject::CanTick() const
{
	return m_isTickable || m_tickableComponents.size() != 0;
}

void GameObject::MarkDirty()
{
	m_boundingBox.reset();
	if (m_quadtreePlacement.IsValid())
	{
		m_quadtreePlacement.InvalidatePlacement();
	}
}

#undef max

float GameObject::TraceRay(const RayIntersectionTester& ray)
{
	float closestDistance = std::numeric_limits<float>::max();

	for (auto it = m_components.begin(); it != m_components.end(); ++it)
	{
		if (const Component* component = (*it).get())
		{
			Ref<SceneComponent> sceneComponent = component->GetRef().Cast<SceneComponent>();
			if (sceneComponent.IsValid())
			{
				const float distance = sceneComponent->TraceRay(ray, closestDistance);
				if (distance >= 0.0f && distance < closestDistance)
				{
					closestDistance = distance;
				}
			}
		}
	}

	return closestDistance;
}

void GameObject::InitComponents()
{
	for (auto it = m_components.begin(); it != m_components.end(); ++it)
	{
		if (Component* component = (*it).get())
		{
			if (!component->GetRef().IsValid())
			{
				component->Init(GetRef(), *it);
			}
		}
	}
}

BoundingBox GameObject::CalculateBoundingBox() const
{
	const Vector3D position    = GetPosition();
	BoundingBox    boundingBox = BoundingBox(position, position);

	for (auto it = m_components.begin(); it != m_components.end(); ++it)
	{
		if (Component* component = (*it).get())
		{
			if (component->GetRef().IsValid())
			{
				boundingBox.Append(component->GetBoundingBox());
			}
		}
	}

	return boundingBox;
}

void GameObject::TickComponents(const float deltaTime)
{
	bool nullElementFound = false;
	for (Component* tickableComponent : m_tickableComponents)
	{
		if (tickableComponent == nullptr)
		{
			nullElementFound = true;
			continue;
		}
		tickableComponent->Tick(deltaTime);
	}

	if (nullElementFound)
	{
		UpdateTickableComponents();
	}
}
