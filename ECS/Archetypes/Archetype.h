#pragma once

#include <vector>

#include "Common/Class.h"

class Entity;

struct Archetype
{
	friend class ArchetypeBuilder;

	struct ComponentTypeInfo
	{
		uint64_t m_typeId;
		uint32_t m_dataSize;
	};

	const std::vector<ComponentTypeInfo> m_componentTypeInfos;
	const std::vector<const Class*> m_classes;

	std::vector<std::vector<uint8_t>> m_componentData;
	std::vector<Entity*> m_entities;

	void AddEntity(Entity* entity);
	void RemoveEntity(Entity* entity);

	uint32_t GetNewEntityOffset(Entity* entity);

	static const std::vector<std::unique_ptr<Archetype>>& GetAllArchetypes() { return m_allArchetypes; }

private:

	explicit Archetype(const std::vector<const Class*>& classes);

	std::priority_queue<uint32_t> m_emptySlots;

	static std::vector<std::unique_ptr<Archetype>> m_allArchetypes;
};
