#pragma once
#include "Plane.h"

struct Frustum
{
	enum class IntersectTestResult
	{
		OUTSIDE,
		INTERSECT,
		INSIDE
	};
	
	Plane m_planes[6];

	IntersectTestResult Intersect(const BoundingBox& boundingBox) const;
};
