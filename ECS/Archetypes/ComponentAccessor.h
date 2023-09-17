#pragma once

struct Archetype;
class Class;
struct Component;
class Entity;

class ComponentAccessor
{
	friend struct Archetype;

public:

	void Setup(Entity* entity);

	ComponentAccessor() = default;

	ComponentAccessor(const ComponentAccessor& other)                = delete;
	ComponentAccessor(ComponentAccessor&& other)                     = delete;
	ComponentAccessor& operator=(const ComponentAccessor& other)     = delete;
	ComponentAccessor& operator=(ComponentAccessor&& other) noexcept = delete;

	template<typename T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
	[[nodiscard]] T* GetComponent() const
	{
		return (T*) GetComponentFromClass(T::GetClass());
	}

	void* GetComponentFromClass(const Class& clazz) const;

	bool IsValid(uint64_t instanceId) const;

private:

	Entity* m_entity             = nullptr;
	const Archetype* m_archetype = nullptr;

	uint32_t m_entityOffset = 0;
};
