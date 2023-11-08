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
		uint32_t m_componentIds[sizeof...(Types)];
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
		m_currentIteration = 0;
		while (m_currentIteration < m_iterationCount)
		{
			if (m_isMultiThreaded)
			{
				std::for_each(std::execution::par_unseq,
					m_archetypesMatched.begin(),
					m_archetypesMatched.end(),
					[this](const MatchedArchetype& archetype)
					{
						std::atomic_uint32_t counter = 0;
						std::for_each(std::execution::par_unseq,
							archetype.m_archetype->m_entities.begin(),
							archetype.m_archetype->m_entities.end(),
							[this, archetype, &counter](const Entity* entity)
							{
								const uint32_t id = counter++;
								if (archetype.m_archetype->m_entities[id] != nullptr)
								{
									Run<0>(archetype, id, archetype.m_archetype->m_entities[id]);
								}
							});
					});
			}
			else
			{
				std::for_each(std::execution::seq,
					m_archetypesMatched.begin(),
					m_archetypesMatched.end(),
					[this](const MatchedArchetype& archetype)
					{
						uint32_t counter = 0;
						std::for_each(std::execution::seq,
							archetype.m_archetype->m_entities.begin(),
							archetype.m_archetype->m_entities.end(),
							[this, archetype, &counter](const Entity* entity)
							{
								const uint32_t id = counter++;
								if (archetype.m_archetype->m_entities[id] != nullptr)
								{
									Run<0>(archetype, id, archetype.m_archetype->m_entities[id]);
								}
							});
					});
			}
			++m_currentIteration;
		}

		OnUpdateEnd();
	}

protected:

	virtual void Update(Entity* entity, Types*... ts) = 0;

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
	std::enable_if_t<Index != sizeof...(Types)> Run(const MatchedArchetype& archetype, const uint32_t id, Entity* entity, Ts... ts)
	{
		typedef std::tuple_element_t<Index, std::tuple<Types...>> ComponentType;

		const uint32_t componentId    = archetype.m_componentIds[Index];
		ComponentType* componentStart = (ComponentType*) archetype.m_archetype->m_componentData[componentId].data();

		Run<Index + 1>(archetype, id, entity, ts..., componentStart + id);
	}

	template<std::size_t Index, typename... Ts>
	std::enable_if_t<Index == sizeof...(Types)> Run(const MatchedArchetype& archetype, const uint32_t id, Entity* entity, Ts... ts)
	{
		Update(entity, ts...);
	}

	std::vector<MatchedArchetype> m_archetypesMatched;

	uint32_t m_archetypesParsed = 0;
};
