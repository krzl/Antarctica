#include "stdafx.h"
#include "Archetype.h"

#include "Entities/Entity.h"

std::vector<std::unique_ptr<Archetype>> Archetype::m_allArchetypes;

static std::vector<Archetype::ComponentTypeInfo> CreateComponentInfos(const std::vector<const Class*>& classes)
{
	std::vector<Archetype::ComponentTypeInfo> infos(classes.size());

	for (uint32_t i = 0; i < classes.size(); ++i)
	{
		infos[i] = { classes[i]->GetId(), (uint32_t) classes[i]->GetDataSize() };
	}

	return infos;
}

Archetype::Archetype(const std::vector<const Class*>& classes) :
	m_componentTypeInfos(CreateComponentInfos(classes)),
	m_classes(classes)
{
	m_componentData.resize(classes.size());
	m_allArchetypes.emplace_back(this);
}

uint32_t Archetype::GetNewEntityOffset(Entity* entity)
{
	if (m_emptySlots.size() == 0)
	{
		m_entities.emplace_back(entity);

		for (uint32_t i = 0; i < m_componentTypeInfos.size(); ++i)
		{
			m_componentData[i].resize(m_componentData[i].size() + m_componentTypeInfos[i].m_dataSize);
		}

		return (uint32_t) m_entities.size() - 1;
	}

	const uint32_t offset = m_emptySlots.top();
	m_emptySlots.pop();
	m_entities[offset] = entity;
	return offset;
}

void Archetype::AddEntity(Entity* entity)
{
	const uint32_t insertPosition = GetNewEntityOffset(entity);

	for (uint32_t i = 0; i < m_componentTypeInfos.size(); ++i)
	{
		const uint32_t dataSize = m_componentTypeInfos[i].m_dataSize;
		void* componentPtr      = m_componentData[i].data() + dataSize * insertPosition;

		m_classes[i]->CreateObjectPlacement(componentPtr);
	}
}

void Archetype::RemoveEntity(Entity* entity)
{
	const uint32_t position = entity->GetComponentAccessor().m_entityOffset;
	m_emptySlots.emplace(position);
	m_entities[position] = nullptr;

	for (uint32_t i = 0; i < m_componentTypeInfos.size(); ++i)
	{
		const uint32_t dataSize = m_componentTypeInfos[i].m_dataSize;
		void* componentPtr      = m_componentData[i].data() + dataSize * position;
		m_classes[i]->DeleteObject(componentPtr);
	}
}
