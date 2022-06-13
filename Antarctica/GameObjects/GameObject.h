#pragma once

#include "Component.h"
#include "Components/SceneComponent.h"

class Class;

class GameObject
{
	friend class World;
	friend class Class;

public:

	GameObject();
	//GameObject(const GameObject& other)            = delete;
	//GameObject& operator=(const GameObject& other) = delete;
	virtual ~GameObject()                          = default;

	// Add Component

	template<
		typename T,
		class = std::enable_if_t<std::is_base_of<Component, T>::value>>
	Ref<T> AddComponent(const Ref<SceneComponent> parent = Ref<SceneComponent>())
	{
		const Class& clazz = T::GetClass();
		return AddComponent(clazz, parent).Cast<T>();
	}

	Ref<Component> AddComponent(const Class& clazz, const Ref<SceneComponent> parent = Ref<SceneComponent>());

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
		const Class& clazz                           = T::GetClass();
		const std::vector<Ref<Component>> components = GetComponentsFromClass(clazz);

		std::vector<Ref<T>> ret(components.size());
		std::transform(components.begin(), components.end(), ret.begin(), [](Ref<Component> ptr)
		{
			return Ref<T>(ptr);
		});
		return ret;
	}

protected:

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

	[[nodiscard]] const Ref<SceneComponent>& GetRoot() const
	{
		return root;
	}

	Point3D GetPosition() const
	{
		return root->GetWorldPosition();
	}

	void SetPosition(const Point3D position)
	{
		root->SetWorldPosition(position);
	}

	Quaternion GetRotation() const
	{
		return root->GetWorldRotation();
	}

	void SetRotation(const Quaternion rotation)
	{
		root->SetWorldRotation(rotation);
	}

	void SetRotation(const Vector3D axis)
	{
		root->SetWorldRotation(axis);
	}

	void SetRotation(const float x, const float y, const float z)
	{
		root->SetWorldRotation(x, y, z);
	}

	Vector3D GetScale() const
	{
		return root->GetWorldScale();
	}

	void SetScale(const Vector3D scale)
	{
		root->SetWorldScale(scale);
	}

	Transform4D GetTransform() const
	{
		return root->GetWorldTransform();
	}

	// Setters

	void SetEnabled(bool isEnabled);

protected:

	// Virtual functions

	virtual void OnCreated()
	{
	}
	virtual void OnEnabled()
	{
	}
	virtual void Tick()
	{
	}
	virtual void OnDisabled()
	{
	}
	virtual void OnDestroy()
	{
	}

	void InitComponents();

private:

	void TickComponents();

	std::unordered_multimap<uint64_t, std::shared_ptr<Component>> m_components;

	const Class* m_class;
	uint64_t m_instanceId;
	World* m_world;

	Ref<GameObject> m_self;

	Ref<SceneComponent> root;

	bool m_isEnabled        = true;
	bool m_isPendingDestroy = false;

	uint32_t m_componentCounter = 0;
};
