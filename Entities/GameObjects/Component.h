#pragma once

#include "Common/Class.h"

class Component
{
	friend class Class;
	friend class GameObject;

public:

	virtual ~Component() = default;

	Component(const Component& other)            = delete;
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

	// ReSharper disable once CppConstValueFunctionReturnType
	[[nodiscard]] const Ref<Component> GetRef() const
	{
		return const_cast<Component*>(this)->GetRef();
	}

	[[nodiscard]] Ref<Component> GetRef()
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
	virtual void Tick(float deltaTime) {}
	virtual void OnDisabled() {}
	virtual void OnDestroy() {}

	Ref<Component> m_self;

	Ref<GameObject> m_owner;
	const Class* m_class;

private:

	void Init(const Ref<GameObject> owner, const Ref<Component> self);

	uint32_t m_componentId = 0;

	bool m_isEnabled = true;
};
