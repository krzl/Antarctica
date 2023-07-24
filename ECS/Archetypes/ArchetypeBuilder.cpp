#include "stdafx.h"
#include "ArchetypeBuilder.h"

#include "Common/Class.h"

struct ClassSorter
{
	bool operator()(const Class* a, const Class* b) const
	{
		return a->GetId() < b->GetId();
	}
};

Archetype* ArchetypeBuilder::Build()
{
	std::sort(m_classes.begin(), m_classes.end(), ClassSorter());

	for (const std::unique_ptr<Archetype>& archetype : Archetype::m_allArchetypes)
	{
		if (archetype->m_componentTypeInfos.size() != m_classes.size())
		{
			continue;
		}

		bool isPerfectMatch = true;
		for (uint32_t i = 0; i < m_classes.size(); ++i)
		{
			if (archetype->m_componentTypeInfos[i].m_typeId != m_classes[i]->GetId())
			{
				isPerfectMatch = false;
				break;
			}
		}

		if (isPerfectMatch)
		{
			return archetype.get();
		}
	}

	Archetype* archetype = new Archetype(m_classes);

	return archetype;
}