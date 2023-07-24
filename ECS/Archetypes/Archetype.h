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

	std::vector<std::vector<uint8_t>> m_componentData;
	std::vector<uint64_t>             m_entityIds;

	void     AddEntity(const Entity* entity, const std::vector<const Class*>& classes);

	static const std::vector<std::unique_ptr<Archetype>>& GetAllArchetypes() { return m_allArchetypes; }

private:

	explicit Archetype(const std::vector<const Class*>& classes);

	static std::vector<std::unique_ptr<Archetype>> m_allArchetypes;
};
