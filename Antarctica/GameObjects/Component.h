#pragma once

#include "Common/Class.h"

class Component
{
	friend class Class;
	friend class GameObject;

public:
	virtual ~Component() = default;

	Component(const Component& other) = delete;
	Component& operator=(const Component& other) = delete;

	// Dispatchers

	Dispatcher<Ref<Component>> OnComponentEnabled;
	Dispatcher<Ref<Component>> OnComponentDisabled;
	Dispatcher<Ref<Component>> OnComponentDestroyed;

	// Getters

	[[nodiscard]] const Class* GetClass() const
	{
		return m_class;
	}

	[[nodiscard]] Ref<GameObject> GetOwner() const
	{
		return m_owner;
	}

	[[nodiscard]] const Ref<Component>& GetRef() const
	{
		return m_self;
	}

	bool IsEnabled() const;

	// Setters

	void SetEnabled(bool isEnabled);

protected:
	
	Component() = default;

	// Virtual functions

	virtual void OnCreated() {}
	virtual void OnEnabled() {}
	virtual void Tick() {}
	virtual void OnDisabled() {}
	virtual void OnDestroy() {}
	
private:

	void Init(const Ref<GameObject> owner, const Ref<Component> self);

	Ref<GameObject> m_owner;
	Ref<Component> m_self;
	const Class* m_class;

	uint32_t m_componentId = 0;
	
	bool m_isEnabled = true;
};
