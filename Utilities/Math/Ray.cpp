#include "stdafx.h"
#include "Ray.h"

Ray Ray::Transform(const Transform4D& transform) const
{
	Ray ray;
	ray.m_origin    = transform * m_origin;
	ray.m_direction = (transform * m_direction).Normalize();

	return ray;
}
