#include "stdafx.h"
#include "Quadtree.h"

#include "GameObjects/GameObject.h"

void Quadtree::PlacementRef::InvalidatePlacement()
{
	if (m_isWaitingForUpdate)
	{
		return;
	}
	m_isWaitingForUpdate = true;
	m_node->m_tree->InvalidatePlacement(this);
}

bool Quadtree::PlacementRef::IsValid() const
{
	return m_node != nullptr;
}

void Quadtree::PlacementRef::Refresh()
{
	m_isWaitingForUpdate = false;
	BoundingBox boundingBox;
	{
		std::lock_guard lock(m_node->m_mutex);
		boundingBox = m_object->GetBoundingBox();
	}

	if (!m_node->Contains(boundingBox))
	{
		Quadtree& tree = m_node->GetTree();
		m_node->RemoveObject(m_object);

		const PlacementRef placement = tree.AddObject(m_object, boundingBox);
		m_node                       = placement.m_node;
	}
	else
	{
		m_node = m_node->TryPushBack(m_object, boundingBox);
	}
}

Quadtree::Quadtree() :
	m_threadPool([](PlacementRef* ref) { ref->Refresh(); }, 8)
{
	m_root = new Node();

	const Vector3D lowerBoundary = Vector3D::zero - Vector3D(INITIAL_ROOT_SIZE, INITIAL_ROOT_SIZE, 0.0f);
	const Vector3D upperBoundary = Vector3D::zero + Vector3D(INITIAL_ROOT_SIZE, INITIAL_ROOT_SIZE, 0.0f);
	m_root->m_boundingBox        = BoundingBox(lowerBoundary, upperBoundary);
	m_root->m_tree               = this;
}

void Quadtree::ExpandRoot(const Vector3D& point)
{
	std::lock_guard lock(m_root->m_mutex);

	const Vector3D halfUp =
		Vector3D(0.0f, m_root->m_boundingBox.m_upperBoundary.y - m_root->m_boundingBox.m_lowerBoundary.y, 0.0f);
	const Vector3D halfRight =
		Vector3D(m_root->m_boundingBox.m_upperBoundary.x - m_root->m_boundingBox.m_lowerBoundary.x, 0.0f, 0.0f);

	Vector3D             midPoint;
	Node::ChildDirection oldRootDirection;

	if (m_root->m_boundingBox.m_lowerBoundary.x >= point.x)
	{
		if (m_root->m_boundingBox.m_lowerBoundary.y >= point.y)
		{
			oldRootDirection = Node::ChildDirection::TOP_RIGHT;
			midPoint         = m_root->m_boundingBox.m_lowerBoundary;
		}
		else
		{
			oldRootDirection = Node::ChildDirection::BOT_RIGHT;
			midPoint         = m_root->m_boundingBox.m_lowerBoundary + halfUp;
		}
	}
	else
	{
		if (m_root->m_boundingBox.m_lowerBoundary.y >= point.y)
		{
			oldRootDirection = Node::ChildDirection::TOP_LEFT;
			midPoint         = m_root->m_boundingBox.m_lowerBoundary + halfRight;
		}
		else
		{
			oldRootDirection = Node::ChildDirection::BOT_LEFT;
			midPoint         = m_root->m_boundingBox.m_upperBoundary;
		}
	}

	const Vector3D lowerBoundary = midPoint - halfUp - halfRight;

	std::pair<Vector3D, Vector3D> childBoundaries[] = {
		{ lowerBoundary, midPoint },
		{ lowerBoundary + halfRight, midPoint + halfRight },
		{ lowerBoundary + halfUp, midPoint + halfUp },
		{ midPoint, midPoint + halfUp + halfRight },
	};

	Node* oldRoot = m_root;

	m_root                     = new Node();
	m_root->m_tree             = this;
	m_root->m_boundingBox      = BoundingBox(childBoundaries[0].first, childBoundaries[3].second);
	m_root->m_totalObjectCount = oldRoot->m_totalObjectCount;
	memcpy(m_root->m_childObjectCount, oldRoot->m_childObjectCount, sizeof oldRoot->m_childObjectCount);

	for (uint32_t i = 0; i < (uint32_t) Node::ChildDirection::COUNT; i++)
	{
		if (i == (uint32_t) oldRootDirection)
		{
			oldRoot->m_parent       = m_root;
			m_root->m_childNodes[i] = oldRoot;
		}
		else
		{
			Node* element          = m_root->m_childNodes[i] = new Node();
			element->m_tree        = this;
			element->m_parent      = m_root;
			element->m_boundingBox = BoundingBox(childBoundaries[i].first, childBoundaries[i].second);
		}
	}
}

void Quadtree::InvalidatePlacement(PlacementRef* placement)
{
	m_threadPool.AddItem(placement);
}

Quadtree::PlacementRef Quadtree::AddObject(GameObject* object, const BoundingBox boundingBox)
{
	while (!m_root->Contains(boundingBox.m_lowerBoundary))
	{
		ExpandRoot(boundingBox.m_lowerBoundary);
	}

	while (!m_root->Contains(boundingBox.m_upperBoundary))
	{
		ExpandRoot(boundingBox.m_upperBoundary);
	}

	return m_root->AddObject(object, boundingBox);
}

void Quadtree::RemoveObject(GameObject* object)
{
	m_threadPool.RemoveItem(&object->m_quadtreePlacement);
	object->m_quadtreePlacement.m_node->RemoveObject(object);
}

void Quadtree::Update()
{
	m_threadPool.WaitForCompletion();
}

bool Quadtree::Node::Contains(const Vector3D& point) const
{
	return m_boundingBox.Contains2D((Point2D) point.xy);
}

bool Quadtree::Node::Contains(const BoundingBox& boundingBox) const
{
	return m_boundingBox.Contains2D(boundingBox);
}

void Quadtree::Node::AddObjectInner(const GameObject* object, const BoundingBox& boundingBox)
{
	const bool shouldCreateChildren = m_totalObjectCount != 1 && m_childNodes[0] == nullptr && (
										  m_boundingBox.m_upperBoundary.x - m_boundingBox.m_lowerBoundary.x) > 1.0f;

	m_boundingBox.m_upperBoundary.z = max(m_boundingBox.m_upperBoundary.z, boundingBox.m_upperBoundary.z);
	m_boundingBox.m_lowerBoundary.z = min(m_boundingBox.m_lowerBoundary.z, boundingBox.m_lowerBoundary.z);

	if (shouldCreateChildren)
	{
		CreateChildren();
	}

	IncrementCollisionCounts(object);
}

Quadtree::PlacementRef Quadtree::Node::AddObject(GameObject* object, const BoundingBox& boundingBox)
{
	{
		std::lock_guard lock(m_mutex);
		AddObjectInner(object, boundingBox);
	}


	for (Node* childNode : m_childNodes)
	{
		if (childNode != nullptr && childNode->Contains(boundingBox))
		{
			return childNode->AddObject(object, boundingBox);
		}
	}

	{
		std::lock_guard lock(m_mutex);
		m_objects.emplace(object);
	}

	return PlacementRef(this, object);
}

void Quadtree::Node::RemoveObject(GameObject* object)
{
	{
		std::lock_guard lock(m_mutex);
		m_objects.erase(object);
	}

	RemoveObjectInner(object);
}

Quadtree::Node* Quadtree::Node::TryPushBack(GameObject* object, const BoundingBox& boundingBox)
{
	if (!boundingBox.Contains2D((Point2D) m_boundingBox.GetCenter().xy) && (
			m_childNodes[0] != nullptr || m_objects.size() > 1))
	{
		{
			std::lock_guard lock(m_mutex);
			m_objects.erase(object);
		}

		DecrementCollisionCounts(object);

		const PlacementRef placement = AddObject(object, boundingBox);
		return placement.m_node;
	}

	return this;
}

void Quadtree::Node::RemoveObjectInner(GameObject* object)
{
	DecrementCollisionCounts(object);

	if (m_parent)
	{
		m_parent->RemoveObjectInner(object);
	}
}

void Quadtree::Node::CreateChildren()
{
	const Vector3D halfUp = Vector3D(0.0f, (m_boundingBox.m_upperBoundary.y - m_boundingBox.m_lowerBoundary.y) / 2.0f,
									 0.0f);
	const Vector3D halfRight = Vector3D((m_boundingBox.m_upperBoundary.x - m_boundingBox.m_lowerBoundary.x) / 2.0f,
										0.0f, 0.0f);

	const Vector3D midPoint = m_boundingBox.m_lowerBoundary + halfUp + halfRight;

	std::pair<Vector3D, Vector3D> childBoundaries[] = {
		{ m_boundingBox.m_lowerBoundary, midPoint },
		{ m_boundingBox.m_lowerBoundary + halfRight, midPoint + halfRight },
		{ m_boundingBox.m_lowerBoundary + halfUp, midPoint + halfUp },
		{ midPoint, m_boundingBox.m_upperBoundary },
	};

	for (uint32_t i = 0; i < (uint32_t) ChildDirection::COUNT; i ++)
	{
		Node* element          = m_childNodes[i] = new Node();
		element->m_tree        = m_tree;
		element->m_parent      = this;
		element->m_boundingBox = BoundingBox(childBoundaries[i].first, childBoundaries[i].second);
	}

	for (auto it = m_objects.begin(); it != m_objects.end();)
	{
		GameObject* object = *it;

		const BoundingBox& boundingBox = object->GetBoundingBox();

		bool wasPlacedInChild = false;

		for (Node* childNode : m_childNodes)
		{
			if (childNode != nullptr && childNode->Contains(boundingBox))
			{
				it                          = m_objects.erase(it);
				object->m_quadtreePlacement = childNode->AddObject(object, boundingBox);
				wasPlacedInChild            = true;
				break;
			}
		}

		if (!wasPlacedInChild)
		{
			++it;
		}
	}
}

void Quadtree::Node::IncrementCollisionCounts(const GameObject* object)
{
	const uint32_t collisionChannel = object->GetCollisionChannel();

	for (uint32_t i = 0; i < 32; i++)
	{
		const Collision::CollisionChannel mask = (Collision::CollisionChannel) (1 << i);

		if ((uint32_t) mask & collisionChannel)
		{
			++m_childObjectCount[(uint32_t) mask];
		}
	}

	++m_totalObjectCount;
}

void Quadtree::Node::DecrementCollisionCountsInner(const GameObject* object)
{
	const uint32_t collisionChannel = object->GetCollisionChannel();

	for (uint32_t i = 0; i < 32; i++)
	{
		const Collision::CollisionChannel mask = (Collision::CollisionChannel) (1 << i);

		if ((uint32_t) mask & collisionChannel)
		{
			--m_childObjectCount[(uint32_t) mask];
		}
	}

	--m_totalObjectCount;

	if (m_totalObjectCount == 0)
	{
		m_boundingBox.m_upperBoundary.z = m_boundingBox.m_lowerBoundary.z = 0.0f;
	}
}

void Quadtree::Node::DecrementCollisionCounts(const GameObject* object)
{
	{
		std::lock_guard lock(m_mutex);
		DecrementCollisionCountsInner(object);
	}
}

#undef max

Quadtree::TraceResult Quadtree::TraceObject(const BoundingBox::RayIntersectionTester& ray) const
{
	const_cast<Quadtree*>(this)->Update();

	float distance = std::numeric_limits<float>::max();

	GameObject* object = m_root->TraceObject(ray, distance);

	if (object != nullptr)
	{
		return {
			object->GetRef(),
			ray.m_ray.m_origin + ray.m_ray.m_direction * distance,
			distance
		};
	}

	return
	{
		object != nullptr ? object->GetRef() : Ref<GameObject>(),
		(Point3D) Vector3D::zero,
		0.0f
	};
}

GameObject* Quadtree::Node::TraceObject(const BoundingBox::RayIntersectionTester& ray, float& minDistance) const
{
	GameObject* closestObject = nullptr;

	for (GameObject* object : m_objects)
	{
		float distance = ray.Intersect(object->GetBoundingBox());

		if (distance < 0.0f || minDistance <= distance)
		{
			continue;
		}

		distance = object->TraceRay(ray);

		if (distance < 0.0f || minDistance <= distance)
		{
			continue;
		}

		minDistance   = distance;
		closestObject = object;
	}

	for (const Node* childNode : m_childNodes)
	{
		if (childNode != nullptr && ray.Intersect(childNode->m_boundingBox) < minDistance)
		{
			float       distance = minDistance;
			GameObject* object   = childNode->TraceObject(ray, distance);
			if (object && distance >= 0.0f && distance < minDistance)
			{
				closestObject = object;
				minDistance   = distance;
			}
		}
	}

	return closestObject;
}

std::vector<GameObject*> Quadtree::Intersect(const Frustum& frustum) const
{
	const_cast<Quadtree*>(this)->Update();

	std::vector<GameObject*> objects;

	m_root->TestIntersect(frustum, objects);

	return objects;
}


void Quadtree::Node::TestIntersect(const Frustum& frustum, std::vector<GameObject*>& objects) const
{
	for (GameObject* object : m_objects)
	{
		if (frustum.Intersect(object->GetBoundingBox()) != Frustum::IntersectTestResult::OUTSIDE)
		{
			objects.emplace_back(object);
		}
	}

	for (const Node* childNode : m_childNodes)
	{
		if (childNode != nullptr && childNode->m_totalObjectCount > 0)
		{
			switch (frustum.Intersect(childNode->m_boundingBox))
			{
				case Frustum::IntersectTestResult::OUTSIDE:
					continue;
				case Frustum::IntersectTestResult::INTERSECT:
					childNode->TestIntersect(frustum, objects);
					break;
				case Frustum::IntersectTestResult::INSIDE:
					childNode->CollectChildObjects(objects);
					break;
			}
		}
	}
}

void Quadtree::Node::CollectChildObjects(std::vector<GameObject*>& objects) const
{
	for (GameObject* object : m_objects)
	{
		objects.emplace_back(object);
	}

	for (const Node* childNode : m_childNodes)
	{
		if (childNode != nullptr && childNode->m_totalObjectCount > 0)
		{
			childNode->CollectChildObjects(objects);
		}
	}
}
