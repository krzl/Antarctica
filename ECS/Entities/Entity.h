#pragma once

#include "Archetypes/Archetype.h"
#include "Archetypes/ComponentAccessor.h"
#include "Quadtree/Quadtree.h"

struct Archetype;
class ArchetypeBuilder;
class Class;

class Entity
{
	friend class World;
	friend class Class;
	friend class Quadtree;

public:

	Entity()          = default;
	virtual ~Entity() = default;

	void Destroy();

	[[nodiscard]] uint64_t GetInstanceId() const { return m_instanceId; }

	[[nodiscard]] const Ref<Entity> GetRef() const { return m_self; }
	[[nodiscard]] Ref<Entity> GetRef() { return m_self; }

	[[nodiscard]] const std::string& GetName() const { return m_name; }
	void SetName(std::string name) { m_name = std::move(name); }

	[[nodiscard]] World& GetWorld() const { return *m_world; }

	[[nodiscard]] const Archetype* GetArchetype() const { return m_archetype; }

	[[nodiscard]] const ComponentAccessor& GetComponentAccessor();

	[[nodiscard]] BoundingBox GetBoundingBox();

protected:

	virtual void DefineArchetype(ArchetypeBuilder& builder) {}
	virtual void SetupComponents(const ComponentAccessor& accessor) {}

private:

	void CreateArchetype();
	void FinalizeArchetype(ArchetypeBuilder& archetypeBuilder);

	Archetype* m_archetype;
	uint64_t m_instanceId;

	ComponentAccessor m_componentAccessor;

	Ref<Entity> m_self;
	World* m_world;

	std::string m_name;
};
