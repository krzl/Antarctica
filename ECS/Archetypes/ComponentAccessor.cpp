#include "stdafx.h"
#include "ComponentAccessor.h"

#include "Entities/Entity.h"

void ComponentAccessor::Setup(Entity* entity)
{
	m_entity    = entity;
	m_archetype = entity->GetArchetype();

	const uint64_t instanceId = entity->GetInstanceId();

	const auto entityPlacement = std::find(m_archetype->m_entityIds.begin(), m_archetype->m_entityIds.end(), instanceId);
	m_entityOffset             = std::distance(entity->GetArchetype()->m_entityIds.begin(), entityPlacement);
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

bool ComponentAccessor::IsValid(const uint64_t instanceId) const
{
	return m_entity != nullptr && m_archetype->m_entityIds[m_entityOffset] == instanceId;
}
