#pragma once

#include "Component.h"

#include "Collisions/CollisionChannels.h"

struct ColliderComponent : Component
{
	BoundingBox m_boundingBox;
	BoundingBox m_transformedBoundingBox;

	uint32_t m_collisionChannel = Collision::CollisionChannel::VISIBILITY;

	DEFINE_CLASS();
};

CREATE_CLASS(ColliderComponent);
