#pragma once

#include "Component.h"

class Class;

class GameObject
{
	friend class World;
	friend class Class;
	
public:

	GameObject();
	GameObject(const GameObject& other)            = delete;
	GameObject& operator=(const GameObject& other) = delete;
	virtual ~GameObject()                          = default;

	// Add Component
	
	template<
		typename T,
		class = std::enable_if_t<std::is_base_of<Component, T>::value>>
	Ref<T> AddComponent()
	{
		const Class& clazz = T::GetClass();
		return AddComponent(clazz);
	}
	
	Ref<Component> AddComponent(const Class& clazz);

	// Get Component
	
	template<
		typename T,
		class = std::enable_if_t<std::is_base_of<Component, T>::value>>
	Ref<T> GetComponent()
	{
		const Class& clazz = T::GetClass();
		return Ref<T>(GetComponentFromClass(clazz));
	}
	
	template<
		typename T,
		class = std::enable_if_t<std::is_base_of<Component, T>::value>>
	std::vector<Ref<T>> GetComponents()
	{
		const Class& clazz = T::GetClass();
		const std::vector<Ref<Component>> components = GetComponentsFromClass(clazz);

		std::vector<Ref<T>> ret(components.size());
		std::transform(components.begin(), components.end(), ret.begin(), [](Ref<Component> ptr)
		{
			return Ref<T>(ptr);
		});
		return ret;
	}
	
private:
	
	Ref<Component> GetComponentFromClass(const Class& clazz);
	std::vector<Ref<Component>> GetComponentsFromClass(const Class& clazz);

public:
	// Remove

	void RemoveComponent(Ref<Component> component);

	void Destroy();
	
	// Dispatchers

	Dispatcher<Ref<GameObject>> OnObjectEnabled;
	Dispatcher<Ref<GameObject>> OnObjectDisabled;
	Dispatcher<Ref<GameObject>> OnObjectDestroyed;

	// Getters
	
	uint64_t GetInstanceId() const
	{
		return m_instanceId;
	}

	const Class* GetClass() const
	{
		return m_class;
	}

	World* GetWorld() const
	{
		return m_world;
	}

	bool IsEnabled() const
	{
		return m_isEnabled;
	}

	bool IsPendingDestroy() const
	{
		return m_isPendingDestroy;
	}
	
	// Setters

	void SetEnabled(bool isEnabled);

protected:

	// Virtual functions

	virtual void OnCreated() {}
	virtual void OnEnabled() {}
	virtual void Tick() {}
	virtual void OnDisabled() {}
	virtual void OnDestroy() {}

	std::unordered_multimap<uint64_t, std::shared_ptr<Component>> m_components;

	Class* m_class;
	uint64_t m_instanceId;
	World* m_world;

	Ref<GameObject> m_self;

	bool m_isEnabled = true;
	bool m_isPendingDestroy = false;
};
