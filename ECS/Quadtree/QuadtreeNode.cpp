#include "stdafx.h"
#include "QuadtreeNode.h"

#include "QuadtreePlacementRef.h"
#include "Entities/Entity.h"


bool QuadtreeNode::Contains(const Vector3D& point) const
{
	return m_boundingBox.Contains2D((Point2D) point.xy);
}

bool QuadtreeNode::Contains(const BoundingBox& boundingBox) const
{
	return m_boundingBox.Contains2D(boundingBox);
}

QuadtreePlacementRef QuadtreeNode::AddEntity(Entity* entity, const BoundingBox& boundingBox)
{
	m_isDirty = true;
	++m_totalObjectCount;

	for (QuadtreeNode* childNode : m_childNodes)
	{
		if (childNode != nullptr && childNode->Contains(boundingBox))
		{
			return childNode->AddEntity(entity, boundingBox);
		}
	}

	PlaceEntity(entity);

	return QuadtreePlacementRef(this, entity);
}

void QuadtreeNode::PlaceEntity(Entity* entity)
{
	std::lock_guard lock(m_mutex);
	m_entities.emplace(entity);
}

void QuadtreeNode::RemoveEntity(Entity* entity)
{
	std::lock_guard lock(m_mutex);
	m_entities.erase(entity);
}

QuadtreeNode* QuadtreeNode::TryPushBack(Entity* entity, const BoundingBox& boundingBox)
{
	if (!boundingBox.Contains2D((Point2D) m_boundingBox.GetCenter().xy) && (m_childNodes[0] != nullptr || m_entities.size() > 1))
	{
		RemoveEntity(entity);

		const QuadtreePlacementRef placement = AddEntity(entity, boundingBox);
		return placement.m_node;
	}

	return this;
}

void QuadtreeNode::CreateChildren()
{
	if (m_boundingBox.m_lowerBoundary.x - m_boundingBox.m_upperBoundary.x < Quadtree::MIN_NODE_SIZE + 0.01f)
	{
		return;
	}

	const auto halfUp    = Vector3D(0.0f, (m_boundingBox.m_upperBoundary.y - m_boundingBox.m_lowerBoundary.y) / 2.0f, 0.0f);
	const auto halfRight = Vector3D((m_boundingBox.m_upperBoundary.x - m_boundingBox.m_lowerBoundary.x) / 2.0f, 0.0f, 0.0f);

	const Vector3D midPoint = m_boundingBox.m_lowerBoundary + halfUp + halfRight;

	std::pair<Vector3D, Vector3D> childBoundaries[] = {
		{ m_boundingBox.m_lowerBoundary, midPoint },
		{ m_boundingBox.m_lowerBoundary + halfRight, midPoint + halfRight },
		{ m_boundingBox.m_lowerBoundary + halfUp, midPoint + halfUp },
		{ midPoint, m_boundingBox.m_upperBoundary },
	};

	for (uint32_t i = 0; i < (uint32_t) ChildDirection::COUNT; ++i)
	{
		QuadtreeNode* element  = m_childNodes[i] = new QuadtreeNode();
		element->m_tree        = m_tree;
		element->m_parent      = this;
		element->m_boundingBox = BoundingBox(childBoundaries[i].first, childBoundaries[i].second);
	}
}

uint32_t QuadtreeNode::UpdateEntityCount()
{
	if (!m_isDirty)
	{
		return m_totalObjectCount;
	}
	uint32_t entityCount = 0;
	if (m_childNodes[0] != nullptr)
	{
		for (uint32_t i = 0; i < (uint32_t) ChildDirection::COUNT; ++i)
		{
			entityCount += m_childNodes[i]->UpdateEntityCount();
		}
	}
	entityCount += (uint32_t) m_entities.size();

	m_totalObjectCount = entityCount;
	return entityCount;
}

void QuadtreeNode::UpdateCulling(const Frustum& frustum, const uint32_t frameId)
{
	m_lastVisibilityTestResult = IntersectTestResult::INTERSECT;
	m_lastSeenFrameId          = frameId;

	for (QuadtreeNode* childNode : m_childNodes)
	{
		if (childNode != nullptr && childNode->m_totalObjectCount > 0)
		{
			switch (Intersect(frustum, childNode->m_boundingBox))
			{
				case IntersectTestResult::OUTSIDE:
					continue;
				case IntersectTestResult::INTERSECT:
					childNode->UpdateCulling(frustum, frameId);
					break;
				case IntersectTestResult::INSIDE:
					childNode->SetVisible(frameId);
					break;
			}
		}
	}
}

void QuadtreeNode::SetVisible(const uint32_t frameId)
{
	m_lastVisibilityTestResult = IntersectTestResult::INSIDE;
	m_lastSeenFrameId          = frameId;

	for (QuadtreeNode* childNode : m_childNodes)
	{
		if (childNode != nullptr && childNode->m_totalObjectCount > 0)
		{
			childNode->SetVisible(frameId);
		}
	}
}

Entity* QuadtreeNode::TraceObject(const RayIntersectionTester& ray, float& minDistance) const
{
	Entity* closestObject = nullptr;

	for (Entity* object : m_entities)
	{
		float distance = ray.Intersect(object->GetBoundingBox());

		if (distance < 0.0f || minDistance <= distance)
		{
			continue;
		}

		minDistance   = distance;
		closestObject = object;
	}

	for (const QuadtreeNode* childNode : m_childNodes)
	{
		if (childNode != nullptr && ray.Intersect(childNode->m_boundingBox) < minDistance)
		{
			float distance = minDistance;
			Entity* object = childNode->TraceObject(ray, distance);
			if (object && distance >= 0.0f && distance < minDistance)
			{
				closestObject = object;
				minDistance   = distance;
			}
		}
	}

	return closestObject;
}

void QuadtreeNode::TestIntersect(const Frustum& frustum, std::vector<Entity*>& objects) const
{
	for (Entity* object : m_entities)
	{
		if (::Intersect(frustum, object->GetBoundingBox()) != IntersectTestResult::OUTSIDE)
		{
			objects.emplace_back(object);
		}
	}

	for (const QuadtreeNode* childNode : m_childNodes)
	{
		if (childNode != nullptr && childNode->m_totalObjectCount > 0)
		{
			switch (::Intersect(frustum, childNode->m_boundingBox))
			{
				case IntersectTestResult::OUTSIDE:
					continue;
				case IntersectTestResult::INTERSECT:
					childNode->TestIntersect(frustum, objects);
					break;
				case IntersectTestResult::INSIDE:
					childNode->CollectChildObjects(objects);
					break;
			}
		}
	}
}

void QuadtreeNode::FindNearby(const Sphere& sphere, std::vector<Entity*>& objects) const
{
	for (Entity* object : m_entities)
	{
		if (IsOverlapping2D(sphere, object->GetBoundingBox()))
		{
			objects.emplace_back(object);
		}
	}

	for (const QuadtreeNode* childNode : m_childNodes)
	{
		if (childNode != nullptr && childNode->m_totalObjectCount > 0)
		{
			switch (Intersect2D(sphere, childNode->m_boundingBox))
			{
				case IntersectTestResult::OUTSIDE:
					continue;
				case IntersectTestResult::INTERSECT:
					childNode->FindNearby(sphere, objects);
					break;
				case IntersectTestResult::INSIDE:
					childNode->CollectChildObjects(objects);
					break;
			}
		}
	}
}

void QuadtreeNode::CollectChildObjects(std::vector<Entity*>& objects) const
{
	for (Entity* object : m_entities)
	{
		objects.emplace_back(object);
	}

	for (const QuadtreeNode* childNode : m_childNodes)
	{
		if (childNode != nullptr && childNode->m_totalObjectCount > 0)
		{
			childNode->CollectChildObjects(objects);
		}
	}
}
