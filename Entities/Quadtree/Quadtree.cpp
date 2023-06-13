#include "stdafx.h"
#include "Quadtree.h"

#include "GameObjects/GameObject.h"

void Quadtree::PlacementRef::InvalidatePlacement()
{
	m_node->m_tree->InvalidatePlacement(this);
}

bool Quadtree::PlacementRef::IsValid() const
{
	return m_node != nullptr;
}

void Quadtree::PlacementRef::Refresh()
{
	const BoundingBox boundingBox = m_object->GetBoundingBox();
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

Quadtree::Quadtree()
{
	m_root = new Node();

	const Vector3D lowerBoundary = Vector3D::zero - Vector3D(INITIAL_ROOT_SIZE, INITIAL_ROOT_SIZE, INITIAL_ROOT_SIZE);
	const Vector3D upperBoundary = Vector3D::zero + Vector3D(INITIAL_ROOT_SIZE, INITIAL_ROOT_SIZE, INITIAL_ROOT_SIZE);
	m_root->m_boundingBox        = BoundingBox(lowerBoundary, upperBoundary);
	m_root->m_tree               = this;

	for (uint32_t i = 0; i < NUM_THREADS; ++i)
	{
		m_threads.emplace_back(std::bind(&Quadtree::ThreadLoop, this));
		m_threads[i].detach();
	}
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
	if constexpr (NUM_THREADS != 0)
	{
		std::lock_guard lock(m_mutex);
		m_updateList.emplace_back(placement);
		m_awaitCV.notify_all();
	}
	else
	{
		m_updateList.emplace_back(placement);
	}
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
	for (auto it = m_updateList.begin(); it != m_updateList.end(); ++it)
	{
		if (*it == &object->m_quadtreePlacement)
		{
			m_updateList.erase(it);
			break;
		}
	}

	m_root->RemoveObject(object);
}

void Quadtree::Update()
{
	if constexpr (NUM_THREADS != 0)
	{
		std::unique_lock lock(m_mutex);
		if (m_updateList.size() != 0)
		{
			m_updateEndCV.wait(lock, [this] { return m_updateList.size() == 0; });
		}
	}
	else
	{
		for (PlacementRef* element : m_updateList)
		{
			element->Refresh();
		}
		m_updateList.clear();
	}
}

void Quadtree::ThreadLoop()
{
	while (true)
	{
		PlacementRef* ref = nullptr;
		{
			std::unique_lock lock(m_mutex);
			m_awaitCV.wait(lock, [this] { return m_updateList.size() != 0; });

			auto it = m_updateList.end() - 1;
			ref     = *it;

			m_updateList.erase(it);
		}

		ref->Refresh();

		m_updateEndCV.notify_one();
	}
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

	m_maxHeight = max(m_maxHeight, boundingBox.m_upperBoundary.z);
	m_minHeight = max(m_minHeight, boundingBox.m_lowerBoundary.z);

	if (shouldCreateChildren)
	{
		CreateChildren();
	}

	IncrementCollisionCounts(object);
}

Quadtree::PlacementRef Quadtree::Node::AddObject(GameObject* object, const BoundingBox& boundingBox)
{
	if constexpr (NUM_THREADS != 0)
	{
		std::lock_guard lock(m_mutex);

		AddObjectInner(object, boundingBox);
	}
	else
	{
		AddObjectInner(object, boundingBox);
	}


	for (Node* childNode : m_childNodes)
	{
		if (childNode != nullptr && childNode->Contains(boundingBox))
		{
			return childNode->AddObject(object, boundingBox);
		}
	}

	if constexpr (NUM_THREADS != 0)
	{
		std::lock_guard lock(m_mutex);
		m_objects.emplace(object);
	}
	else
	{
		m_objects.emplace(object);
	}

	return PlacementRef(this, object);
}

void Quadtree::Node::RemoveObject(GameObject* object)
{
	if constexpr (NUM_THREADS != 0)
	{
		std::lock_guard lock(m_mutex);
		m_objects.erase(object);
	}
	else
	{
		m_objects.erase(object);
	}
	
	RemoveObjectInner(object);
}

Quadtree::Node* Quadtree::Node::TryPushBack(GameObject* object, const BoundingBox& boundingBox)
{
	if (!boundingBox.Contains2D((Point2D) m_boundingBox.GetCenter().xy) && (
			m_childNodes[0] != nullptr || m_objects.size() > 1))
	{
		if constexpr (NUM_THREADS != 0)
		{
			std::lock_guard lock(m_mutex);
			m_objects.erase(object);
		}
		else
		{
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
		m_maxHeight = m_minHeight = 0.0f;
	}
}

void Quadtree::Node::DecrementCollisionCounts(const GameObject* object)
{
	if constexpr (NUM_THREADS != 0)
	{
		std::lock_guard lock(m_mutex);
		DecrementCollisionCountsInner(object);
	}
	else
	{
		DecrementCollisionCountsInner(object);
	}
}
