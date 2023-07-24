#pragma once

#include "SystemBase.h"
#include "Archetypes/Archetype.h"

class Entity;
struct Archetype;

template<typename... Types>
class System : public SystemBase
{
	struct MatchedArchetype
	{
		const Archetype* m_archetype;
		uint32_t         m_componentIds[sizeof...(Types)];
	};

public:

	void Run() override
	{
		OnUpdateStart();
		const std::vector<std::unique_ptr<Archetype>>& allArchetypes = Archetype::GetAllArchetypes();
		while (allArchetypes.size() != m_archetypesParsed)
		{
			static uint32_t componentIds[sizeof...(Types)];
			TryAddNewArchetype<0>(allArchetypes[m_archetypesParsed++].get(), componentIds);
		}

		for (const MatchedArchetype& archetype : m_archetypesMatched)
		{
			for (uint32_t i = 0; i < archetype.m_archetype->m_entityIds.size(); ++i)
			{
				Run<0>(archetype, i);
			}
		}
		OnUpdateEnd();
	}

protected:

	virtual void Update(uint64_t entityId, Types*... ts) = 0;

private:

	template<std::size_t Index>
	std::enable_if_t<Index != sizeof...(Types)> TryAddNewArchetype(const Archetype* archetype, uint32_t* componentIds)
	{
		using ComponentType = std::tuple_element_t<Index, std::tuple<Types...>>;

		const uint64_t typeId = ComponentType::GetClass().GetId();

		for (uint32_t i = 0; i < archetype->m_componentTypeInfos.size(); ++i)
		{
			const uint64_t archetypeTypeId = archetype->m_componentTypeInfos[i].m_typeId;

			if (typeId == archetypeTypeId)
			{
				componentIds[Index] = i;
				TryAddNewArchetype<Index + 1>(archetype, componentIds);
				return;
			}
		}
	}

	template<std::size_t Index>
	std::enable_if_t<Index == sizeof...(Types)> TryAddNewArchetype(const Archetype* archetype, uint32_t* componentIds)
	{
		MatchedArchetype matchedArchetype;
		matchedArchetype.m_archetype = archetype;
		memcpy((void*) matchedArchetype.m_componentIds, componentIds, sizeof matchedArchetype.m_componentIds);
		m_archetypesMatched.emplace_back(std::move(matchedArchetype));
	}


	template<std::size_t Index, typename... Ts>
	std::enable_if_t<Index != sizeof...(Types)> Run(const MatchedArchetype& archetype, const uint32_t id, Ts... ts)
	{
		using ComponentType = std::tuple_element_t<Index, std::tuple<Types...>>;

		const uint32_t componentId    = archetype.m_componentIds[Index];
		ComponentType* componentStart = (ComponentType*) archetype.m_archetype->m_componentData[componentId].data();

		Run<Index + 1>(archetype, id, ts..., componentStart + id);
	}

	template<std::size_t Index, typename... Ts>
	std::enable_if_t<Index == sizeof...(Types)> Run(const MatchedArchetype& archetype, const uint32_t id, Ts... ts)
	{
		Update(archetype.m_archetype->m_entityIds[id], ts...);
	}

	std::vector<MatchedArchetype> m_archetypesMatched;

	uint32_t m_archetypesParsed = 0;
};
