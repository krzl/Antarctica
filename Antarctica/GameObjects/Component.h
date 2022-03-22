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

	Dispatcher<std::weak_ptr<Component>> OnComponentEnabled;
	Dispatcher<std::weak_ptr<Component>> OnComponentDisabled;
	Dispatcher<std::weak_ptr<Component>> OnComponentDestroyed;

	// Getters

	const Class* GetClass() const
	{
		return m_class;
	}

	std::weak_ptr<GameObject> GetOwner() const
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

	void Init(const std::weak_ptr<GameObject> owner, const std::weak_ptr<Component> weakPtr);

	std::weak_ptr<GameObject> m_owner;
	std::weak_ptr<Component> m_self;
	Class* m_class;

	bool m_isEnabled = true;
};
