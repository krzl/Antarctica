#pragma once

class Component
{
	friend class Class;
	friend class GameObject;

public:

	Component(const Component& other) = delete;
	Component& operator=(const Component& other) = delete;
	virtual ~Component() = default;

	// Dispatchers

	Dispatcher<Ref<Component>> OnComponentEnabled;
	Dispatcher<Ref<Component>> OnComponentDisabled;
	Dispatcher<Ref<Component>> OnComponentDestroyed;

	// Getters

	const Class* GetClass() const
	{
		return m_class;
	}

	Ref<GameObject> GetOwner() const
	{
		return m_owner;
	}

	bool IsEnabled() const;

	// Setters

	void SetEnabled(bool isEnabled);

protected:

	// Virtual functions

	virtual void OnCreated() {}
	virtual void OnEnabled() {}
	virtual void Tick() {}
	virtual void OnDisabled() {}
	virtual void OnDestroy() {}
	
private:

	void Init(const Ref<GameObject> owner, const Ref<Component> weakPtr);

	Ref<GameObject> m_owner;
	Ref<Component> m_self;
	Class* m_class;

	bool m_isEnabled = true;
};
