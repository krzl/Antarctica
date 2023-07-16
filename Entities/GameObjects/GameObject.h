#pragma once

#include "Component.h"
#include "Collisions/CollisionChannels.h"
#include "Components/SceneComponent.h"
#include "Quadtree/Quadtree.h"

class Class;

class GameObject
{
	friend class World;
	friend class Class;
	friend class Quadtree;

public:

	GameObject();
	virtual ~GameObject() = default;

	// Add Component

	template<
		typename T,
		class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	Ref<T> AddComponent(const Ref<SceneComponent> parent = Ref<SceneComponent>())
	{
		const Class& clazz = T::GetClass();
		return AddComponent(clazz, parent).Cast<T>();
	}

	Ref<Component> AddComponent(const Class& clazz, const Ref<SceneComponent> parent = Ref<SceneComponent>());

	// Get Component

	template<
		typename T,
		class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	T* GetComponent()
	{
		for (auto it = m_components.begin(); it != m_components.end(); ++it)
		{
			Component* comp = it->get();
			if (T* ptr = dynamic_cast<T*>(comp))
			{
				return ptr;
			}
		}

		return nullptr;
	}

	template<
		typename T,
		class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	Ref<T> GetComponentRef()
	{
		for (auto it = m_components.begin(); it != m_components.end(); ++it)
		{
			Ref<T> ref = (*it)->m_self.Cast<T>();
			if (ref.IsValid())
			{
				return ref;
			}
		}

		return Ref<T>();
	}

	template<
		typename T,
		class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	std::vector<T*> GetComponents()
	{
		std::vector<T*> components;
		GetComponents<T>(components);
		return components;
	}

	template<
		typename T,
		class = std::enable_if_t<std::is_base_of_v<Component, T>>>
	void GetComponents(std::vector<T*>& components)
	{
		for (auto it = m_components.begin(); it != m_components.end(); ++it)
		{
			if (T* ptr = *(*it)->m_self.Cast<T>())
			{
				components.emplace_back(ptr);
			}
		}
	}

	[[nodiscard]] const std::vector<std::shared_ptr<Component>>& GetComponents() const //TODO: Remove
	{
		return m_components;
	}

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
		return m_root;
	}

	[[nodiscard]] Ref<SceneComponent> GetRoot()
	{
		return m_root;
	}

	Point3D GetPosition() const
	{
		return m_root->GetWorldPosition();
	}

	Quaternion GetRotation() const
	{
		return m_root->GetWorldRotation();
	}

	Vector3D GetScale() const
	{
		return m_root->GetWorldScale();
	}

	Transform4D GetTransform() const
	{
		return m_root->GetWorldTransform();
	}

	// ReSharper disable once CppConstValueFunctionReturnType
	[[nodiscard]] const Ref<GameObject> GetRef() const
	{
		return const_cast<GameObject*>(this)->GetRef();
	}

	[[nodiscard]] Ref<GameObject> GetRef()
	{
		return m_self;
	}

	[[nodiscard]] uint32_t GetCollisionChannel() const
	{
		return m_collisionChannel;
	}

	[[nodiscard]] const std::string& GetName() const
	{
		return m_name;
	}

	BoundingBox GetBoundingBox() const;

	[[nodiscard]] World& GetWorld() const { return *m_world; }

	bool CanTick() const;

	void MarkDirty();

	float TraceRay(const RayIntersectionTester& ray);

	// Setters

	void SetEnabled(bool isEnabled);

	void SetPosition(const Point3D position)
	{
		m_root->SetWorldPosition(position);
	}

	void SetRotation(const Quaternion rotation)
	{
		m_root->SetWorldRotation(rotation);
	}

	void SetRotation(const Vector3D axis)
	{
		m_root->SetWorldRotation(axis);
	}

	void SetRotation(const float x, const float y, const float z)
	{
		m_root->SetWorldRotation(x, y, z);
	}

	void SetScale(const Vector3D scale)
	{
		m_root->SetWorldScale(scale);
	}

	void SetCollisionChannel(const Collision::CollisionChannel collisionChannel)
	{
		m_collisionChannel = (uint32_t) collisionChannel;
	}

	void SetCollisionChannel(const uint32_t collisionChannel)
	{
		m_collisionChannel = collisionChannel;
	}

	void SetName(std::string name)
	{
		m_name = std::move(name);
	}

protected:

	// Virtual functions

	virtual void OnCreated() { }
	virtual void OnEnabled() { }
	virtual void Tick(float deltaTime) { }
	virtual void OnDisabled() { }
	virtual void OnDestroy() { }

	void InitComponents();

	Ref<Component>              GetComponentFromClass(const Class& clazz);
	std::vector<Ref<Component>> GetComponentsFromClass(const Class& clazz);

	bool m_isTickable = false;

	uint32_t m_collisionChannel = (uint32_t) Collision::CollisionChannel::DEFAULT;

private:

	void TickInner(const float deltaTime) { if (m_isTickable) { Tick(deltaTime); } }

	[[nodiscard]] BoundingBox CalculateBoundingBox() const;

	void UpdateTickableComponents();

	void TickComponents(float deltaTime);

	std::vector<std::shared_ptr<Component>> m_components;
	std::vector<Component*>                 m_tickableComponents;

	const Class* m_class;
	uint64_t     m_instanceId;

	Ref<GameObject> m_self;

	Ref<SceneComponent> m_root;

	std::string m_name;

	World* m_world;

	Quadtree::PlacementRef             m_quadtreePlacement;
	mutable std::optional<BoundingBox> m_boundingBox;

	bool m_isEnabled        = true;
	bool m_isPendingDestroy = false;

	uint32_t m_componentCounter = 0;
};
