#pragma once

#include "Component.h"

struct ColliderComponent : Component
{
	BoundingBox m_boundingBox;
	BoundingBox m_transformedBoundingBox;

	DEFINE_CLASS();
};

CREATE_CLASS(ColliderComponent);
