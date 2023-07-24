#include "stdafx.h"
#include "QuadtreePlacementRef.h"

#include "Quadtree.h"
#include "QuadtreeNode.h"

void QuadtreePlacementRef::Refresh(const BoundingBox& boundingBox)
{
	if (!m_node->Contains(boundingBox))
	{
		m_node->m_isDirty = true;

		QuadtreeNode* parent = m_node->m_parent;
		while (parent != nullptr)
		{
			parent->m_isDirty = true;
			parent            = parent->m_parent;
		}

		Quadtree& tree = m_node->GetTree();
		m_node->RemoveEntity(m_object);

		const QuadtreePlacementRef placement = tree.AddEntity(m_object, boundingBox);
		m_node                               = placement.m_node;
	}
	else
	{
		m_node = m_node->TryPushBack(m_object, boundingBox);
	}
}
