#include "stdafx.h"
#include "Entity.h"

#include "World.h"

#include "Archetypes/ArchetypeBuilder.h"
#include "Components/ColliderComponent.h"
#include "Components/TransformComponent.h"

//TODO: remove windows header from unnecessary files
#undef max

void Entity::FinalizeArchetype(ArchetypeBuilder& archetypeBuilder)
{
	m_archetype = archetypeBuilder.Build();
	m_archetype->AddEntity(this);
}

void Entity::CreateArchetype()
{
	ArchetypeBuilder builder;
	DefineArchetype(builder);
	FinalizeArchetype(builder);
}

const ComponentAccessor& Entity::GetComponentAccessor()
{
	if (!m_componentAccessor.IsValid())
	{
		m_componentAccessor.Setup(this);
	}

	return m_componentAccessor;
}

BoundingBox Entity::GetBoundingBox()
{
	PERF_COUNTER(EntityBoundingBoxGather)
	const ComponentAccessor& accessor = GetComponentAccessor();
	if (ColliderComponent* collider = accessor.GetComponent<ColliderComponent>())
	{
		return collider->m_transformedBoundingBox;
	}

	if (const TransformComponent* transform = accessor.GetComponent<TransformComponent>())
	{
		return BoundingBox(transform->m_localPosition, transform->m_localPosition);
	}

	return BoundingBox();
}

void Entity::Destroy()
{
	World::Get()->AddToPendingDestroy(m_self);
	m_archetype->RemoveEntity(this);
}
