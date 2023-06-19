#include "stdafx.h"
#include "Frustum.h"

Frustum::IntersectTestResult ClassifyPlane(const BoundingBox& boundingBox, const Plane& plane)
{
	/*const Point3D center = boundingBox.GetCenter();

	const Vector3D halfSize = center - boundingBox.m_lowerBoundary;

	const float r = abs(halfSize.x * plane.m_direction.x)
					+ abs(halfSize.y * plane.m_direction.y)
					+ abs(halfSize.z * plane.m_direction.z);

	const float d = Dot(plane.m_direction, center) + plane.m_distance;

	if (abs(d) < r)
	{
		return 0.0f;
	}
	if (d < 0.0f)
	{
		return d + r;
	}
	return d - r;
    */


	Vector3D vMin, vMax;

	if (plane.m_direction.x > 0)
	{
		vMin.x = boundingBox.m_lowerBoundary.x;
		vMax.x = boundingBox.m_upperBoundary.x;
	}
	else
	{
		vMin.x = boundingBox.m_upperBoundary.x;
		vMax.x = boundingBox.m_lowerBoundary.x;
	}
	// Y axis 
	if (plane.m_direction.y > 0)
	{
		vMin.y = boundingBox.m_lowerBoundary.y;
		vMax.y = boundingBox.m_upperBoundary.y;
	}
	else
	{
		vMin.y = boundingBox.m_upperBoundary.y;
		vMax.y = boundingBox.m_lowerBoundary.y;
	}
	// Z axis 
	if (plane.m_direction.z > 0)
	{
		vMin.z = boundingBox.m_lowerBoundary.z;
		vMax.z = boundingBox.m_upperBoundary.z;
	}
	else
	{
		vMin.z = boundingBox.m_upperBoundary.z;
		vMax.z = boundingBox.m_lowerBoundary.z;
	}
	if (Dot(plane.m_direction, vMin) + plane.m_distance > 0)
		return Frustum::IntersectTestResult::OUTSIDE;
	if (Dot(plane.m_direction, vMax) + plane.m_distance >= 0)
		return Frustum::IntersectTestResult::INTERSECT;

	return Frustum::IntersectTestResult::INSIDE;
}


Frustum::IntersectTestResult Frustum::Intersect(const BoundingBox& boundingBox) const
{
	IntersectTestResult result = IntersectTestResult::INSIDE;

	for (int i = 0; i < 6; ++i)
	{
		// ReSharper disable once CppIncompleteSwitchStatement
		switch (ClassifyPlane(boundingBox, m_planes[i]))
		{
			case IntersectTestResult::OUTSIDE:
				return IntersectTestResult::OUTSIDE;
			case IntersectTestResult::INTERSECT:
				result = IntersectTestResult::INTERSECT;
				break;
		}
	}

	return result;
}
