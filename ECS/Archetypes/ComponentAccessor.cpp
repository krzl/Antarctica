#include "stdafx.h"
#include "ComponentAccessor.h"

#include "Entities/Entity.h"

void ComponentAccessor::Setup(Entity* entity)
{
	m_entity    = entity;
	m_archetype = entity->GetArchetype();

	const auto entityPlacement = std::find(m_archetype->m_entities.begin(), m_archetype->m_entities.end(), entity);
	m_entityOffset             = std::distance(entity->GetArchetype()->m_entities.begin(), entityPlacement);
}

void* ComponentAccessor::GetComponentFromClass(const Class& clazz) const
{
	for (uint32_t i = 0; i < m_archetype->m_componentTypeInfos.size(); ++i)
	{
		const Archetype::ComponentTypeInfo& typeInfo = m_archetype->m_componentTypeInfos[i];
		if (typeInfo.m_typeId == clazz.GetId())
		{
			return (uint8_t*) m_archetype->m_componentData[i].data() + typeInfo.m_dataSize * m_entityOffset;
		}
	}

	return nullptr;
}

bool ComponentAccessor::IsValid() const
{
	return m_entity != nullptr && m_archetype->m_entities[m_entityOffset] == m_entity;
}
