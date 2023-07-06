#include "stdafx.h"
#include "QuadtreeTester.h"

#include "GameObjects/World.h"

QuadtreeTester::QuadtreeTester()
{
	m_isTickable = true;
}

static void PrintNode(Quadtree::Node* node, const uint32_t depth)
{
	std::ostringstream string;
	for (uint32_t i = 0; i < depth * 2; ++i)
	{
		string << "\t";
	}

	string << "Node: (" << node->m_boundingBox.m_lowerBoundary.x << "," << node->m_boundingBox.m_lowerBoundary.y <<
		") - (" << node->m_boundingBox.m_upperBoundary.x << "," << node->m_boundingBox.m_upperBoundary.y << ")";
	string << " Counter: " << node->m_totalObjectCount;

	std::cout << string.str() << std::endl;

	for (auto gameObject : node->m_objects)
	{
		std::ostringstream string2;
		for (uint32_t i = 0; i < depth * 2 + 1; ++i)
		{
			string2 << "\t";
		}
		string2 << gameObject->GetName();

		std::cout << string2.str() << std::endl;
	}

	if (node->m_childNodes[0] != nullptr)
	{
		for (uint32_t i = 0; i < 4; ++i)
		{
			PrintNode(node->m_childNodes[i], depth + 1);
		}
	}
}

void QuadtreeTester::Tick(const float deltaTime)
{
	GameObject::Tick(deltaTime);

	const Quadtree& quadTree = GetWorld().GetQuadtree();

	Quadtree::Node* node = quadTree.GetRoot();

	system("CLS");
	PrintNode(node, 0);
}
