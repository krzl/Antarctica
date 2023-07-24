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

void Archetype::AddEntity(const Entity* entity, const std::vector<const Class*>& classes)
{
	m_entityIds.emplace_back(entity->GetInstanceId());

	for (uint32_t i = 0; i < m_componentTypeInfos.size(); ++i)
	{
		m_componentData[i].resize(m_componentData[i].size() + m_componentTypeInfos[i].m_dataSize);

		const uint32_t dataSize     = m_componentTypeInfos[i].m_dataSize;
		void*          componentPtr = m_componentData[i].data() + dataSize * (m_entityIds.size() - 1);

		classes[i]->CreateObjectPlacement(componentPtr);
	}
}

Archetype::Archetype(const std::vector<const Class*>& classes) :
	m_componentTypeInfos(CreateComponentInfos(classes))
{
	m_componentData.resize(classes.size());
	m_allArchetypes.emplace_back(this);
}
