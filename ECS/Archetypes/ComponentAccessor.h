#pragma once

struct Archetype;
class Class;
struct Component;
class Entity;

class ComponentAccessor
{
public:

	explicit ComponentAccessor(Entity* entity);

	template<typename T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
	[[nodiscard]] T* GetComponent()
	{
		return (T*) GetComponentFromClass(T::GetClass());
	}

	void* GetComponentFromClass(const Class& clazz) const;

private:

	Entity* m_entity;

	uint32_t         m_entityOffset;
	const Archetype* m_archetype;
};
