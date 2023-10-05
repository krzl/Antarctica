#include "stdafx.h"
#include "QuadtreeUpdateSystem.h"

#include "Components/ColliderComponent.h"
#include "Components/MoveableComponent.h"
#include "Components/TransformComponent.h"
#include "Entities/World.h"

void QuadtreeUpdateSystem::OnUpdateEnd()
{
	World::Get()->GetQuadtree().UpdateEntityCounts();
}

void QuadtreeUpdateSystem::Update(Entity* entity, TransformComponent* transform, MoveableComponent* moveable, ColliderComponent* collider)
{
	collider->m_transformedBoundingBox = collider->m_boundingBox.Transform(transform->GetWorldTransform());
	transform->m_quadtreePlacement.Refresh(collider->m_transformedBoundingBox);
}
