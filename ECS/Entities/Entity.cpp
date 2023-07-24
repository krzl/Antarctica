#include "stdafx.h"
#include "Entity.h"

#include "Archetypes/ArchetypeBuilder.h"
#include "Components/ColliderComponent.h"
#include "Components/TransformComponent.h"

//TODO: remove windows header from unnecessary files
#undef max

void Entity::FinalizeArchetype(ArchetypeBuilder& archetypeBuilder)
{
	m_archetype = archetypeBuilder.Build();
	m_archetype->AddEntity(this, archetypeBuilder.m_classes);
}

void Entity::CreateArchetype()
{
	ArchetypeBuilder builder;
	DefineArchetype(builder);
	FinalizeArchetype(builder);
}

BoundingBox Entity::GetBoundingBox()
{
	ComponentAccessor accessor = GetComponentAccessor();
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
