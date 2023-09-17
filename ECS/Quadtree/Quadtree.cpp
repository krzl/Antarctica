#include "stdafx.h"
#include "Quadtree.h"

#include "QuadtreeNode.h"
#include "QuadtreePlacementRef.h"
#include "Components/TransformComponent.h"
#include "Entities/Entity.h"

Quadtree::Quadtree()
{
	m_root = new QuadtreeNode();

	const Vector3D lowerBoundary = Vector3D::zero - Vector3D(QUADTREE_SIZE, QUADTREE_SIZE, 0.0f);
	const Vector3D upperBoundary = Vector3D::zero + Vector3D(QUADTREE_SIZE, QUADTREE_SIZE, 0.0f);
	m_root->m_boundingBox        = BoundingBox(lowerBoundary, upperBoundary);
	m_root->m_tree               = this;

	m_root->CreateChildren();
}

QuadtreePlacementRef Quadtree::AddEntity(Entity* entity, const BoundingBox boundingBox)
{
	while (!m_root->Contains(boundingBox.m_lowerBoundary) ||
		!m_root->Contains(boundingBox.m_upperBoundary))
	{
		m_root->m_isDirty = true;
		m_root->PlaceEntity(entity);

		return QuadtreePlacementRef(m_root, entity);
	}

	return m_root->AddEntity(entity, boundingBox);
}


void Quadtree::RemoveObject(Entity* entity)
{
	const ComponentAccessor& accessor = entity->GetComponentAccessor();
	if (const TransformComponent* transform = accessor.GetComponent<TransformComponent>())
	{
		QuadtreeNode* node = transform->m_quadtreePlacement.m_node;
		node->m_isDirty    = true;

		QuadtreeNode* parent = node->m_parent;
		while (parent != nullptr)
		{
			parent->m_isDirty = true;
			parent            = parent->m_parent;
		}

		node->RemoveEntity(entity);
	}
}

void Quadtree::UpdateEntityCounts()
{
	m_root->UpdateEntityCount();
}

void Quadtree::CalculateCulling(const Frustum& frustum, const uint32_t frameId)
{
	m_root->UpdateCulling(frustum, frameId);
}

#undef max

Quadtree::TraceResult Quadtree::TraceObject(const RayIntersectionTester& ray) const
{
	float distance = std::numeric_limits<float>::max();

	Entity* object = m_root->TraceObject(ray, distance);

	if (object != nullptr)
	{
		return { object->GetRef(), ray.m_ray.m_origin + ray.m_ray.m_direction * distance, distance };
	}

	return { object != nullptr ? object->GetRef() : Ref<Entity>(), (Point3D) Vector3D::zero, 0.0f };
}

std::vector<Entity*> Quadtree::Intersect(const Frustum& frustum) const
{
	std::vector<Entity*> objects;

	m_root->TestIntersect(frustum, objects);

	return objects;
}


std::vector<Entity*> Quadtree::FindNearby(const Sphere& sphere) const
{
	std::vector<Entity*> objects;
	objects.reserve(100);

	m_root->FindNearby(sphere, objects);
	objects.shrink_to_fit();

	return objects;
}
