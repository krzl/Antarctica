#pragma once
#include "Archetype.h"

class Class;
struct Component;

class ArchetypeBuilder
{
	friend class Entity;

public:

	template<typename T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
	void AddComponent()
	{
		const Class& clazz = T::GetClass();
		m_classes.emplace_back(&clazz);
	}

	Archetype* Build();

private:

	ArchetypeBuilder() = default;

	std::vector<const Class*> m_classes;
};
