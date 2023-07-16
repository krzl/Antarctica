#include "stdafx.h"
#include "Intersections.h"

#include "Sphere.h"

IntersectTestResult Intersect2D(const Sphere& sphere, const BoundingBox& boundingBox)
{
	const Point2D topLeft  = Point2D(boundingBox.m_lowerBoundary.x, boundingBox.m_upperBoundary.y);
	const Point2D topRight = Point2D(boundingBox.m_upperBoundary.x, boundingBox.m_upperBoundary.y);
	const Point2D botLeft  = Point2D(boundingBox.m_lowerBoundary.x, boundingBox.m_lowerBoundary.y);
	const Point2D botRight = Point2D(boundingBox.m_upperBoundary.x, boundingBox.m_lowerBoundary.y);

	if (sphere.Contains(topLeft) &&
		sphere.Contains(topRight) &&
		sphere.Contains(botLeft) &&
		sphere.Contains(botRight))
	{
		return IntersectTestResult::INSIDE;
	}

	Point2D closestPoint;

	if (sphere.m_center.x < boundingBox.m_lowerBoundary.x)
		closestPoint.x = boundingBox.m_lowerBoundary.x;
	else if (sphere.m_center.x > boundingBox.m_upperBoundary.x)
		closestPoint.x = boundingBox.m_upperBoundary.x;
	else
		closestPoint.x = sphere.m_center.x;

	if (sphere.m_center.y < boundingBox.m_lowerBoundary.y)
		closestPoint.y = boundingBox.m_lowerBoundary.y;
	else if (sphere.m_center.y > boundingBox.m_upperBoundary.y)
		closestPoint.y = boundingBox.m_upperBoundary.y;
	else
		closestPoint.y = sphere.m_center.y;

	const Vector2D offset   = sphere.m_center.GetPoint2D() - closestPoint;
	const float    distance = offset.x * offset.x + offset.y * offset.y;

	if (distance <= sphere.m_radius * sphere.m_radius)
	{
		return IntersectTestResult::INTERSECT;
	}

	return IntersectTestResult::OUTSIDE;
}

IntersectTestResult Intersect(const Sphere& sphere, const BoundingBox& boundingBox)
{
	const Point3D topLeftNear =
		Point3D(boundingBox.m_lowerBoundary.x, boundingBox.m_upperBoundary.y, boundingBox.m_lowerBoundary.z);
	const Point3D topRightNear =
		Point3D(boundingBox.m_upperBoundary.x, boundingBox.m_upperBoundary.y, boundingBox.m_lowerBoundary.z);
	const Point3D botLeftNear =
		Point3D(boundingBox.m_lowerBoundary.x, boundingBox.m_lowerBoundary.y, boundingBox.m_lowerBoundary.z);
	const Point3D botRightNear =
		Point3D(boundingBox.m_upperBoundary.x, boundingBox.m_lowerBoundary.y, boundingBox.m_lowerBoundary.z);
	const Point3D topLeftFar =
		Point3D(boundingBox.m_lowerBoundary.x, boundingBox.m_upperBoundary.y, boundingBox.m_upperBoundary.z);
	const Point3D topRightFar =
		Point3D(boundingBox.m_upperBoundary.x, boundingBox.m_upperBoundary.y, boundingBox.m_upperBoundary.z);
	const Point3D botLeftFar =
		Point3D(boundingBox.m_lowerBoundary.x, boundingBox.m_lowerBoundary.y, boundingBox.m_upperBoundary.z);
	const Point3D botRightFar =
		Point3D(boundingBox.m_upperBoundary.x, boundingBox.m_lowerBoundary.y, boundingBox.m_upperBoundary.z);

	if (sphere.Contains(topLeftNear) &&
		sphere.Contains(topRightNear) &&
		sphere.Contains(botLeftNear) &&
		sphere.Contains(botRightNear) &&
		sphere.Contains(topLeftFar) &&
		sphere.Contains(topRightFar) &&
		sphere.Contains(botLeftFar) &&
		sphere.Contains(botRightFar))
	{
		return IntersectTestResult::INSIDE;
	}

	Point3D closestPoint;

	if (sphere.m_center.x < boundingBox.m_lowerBoundary.x)
		closestPoint.x = boundingBox.m_lowerBoundary.x;
	else if (sphere.m_center.x > boundingBox.m_upperBoundary.x)
		closestPoint.x = boundingBox.m_upperBoundary.x;
	else
		closestPoint.x = sphere.m_center.x;

	if (sphere.m_center.y < boundingBox.m_lowerBoundary.y)
		closestPoint.y = boundingBox.m_lowerBoundary.y;
	else if (sphere.m_center.y > boundingBox.m_upperBoundary.y)
		closestPoint.y = boundingBox.m_upperBoundary.y;
	else
		closestPoint.y = sphere.m_center.y;

	if (sphere.m_center.z < boundingBox.m_lowerBoundary.z)
		closestPoint.z = boundingBox.m_lowerBoundary.z;
	else if (sphere.m_center.z > boundingBox.m_upperBoundary.z)
		closestPoint.z = boundingBox.m_upperBoundary.z;
	else
		closestPoint.z = sphere.m_center.z;

	const Vector3D offset   = sphere.m_center - closestPoint;
	const float    distance = Dot(offset, offset);

	if (distance <= sphere.m_radius * sphere.m_radius)
	{
		return IntersectTestResult::INTERSECT;
	}

	return IntersectTestResult::OUTSIDE;
}

static IntersectTestResult ClassifyPlane(const BoundingBox& boundingBox, const Plane& plane)
{
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
		return IntersectTestResult::OUTSIDE;
	if (Dot(plane.m_direction, vMax) + plane.m_distance >= 0)
		return IntersectTestResult::INTERSECT;

	return IntersectTestResult::INSIDE;
}

IntersectTestResult Intersect(const Frustum& frustum, const BoundingBox& boundingBox)
{
	IntersectTestResult result = IntersectTestResult::INSIDE;

	for (int i = 0; i < 6; ++i)
	{
		// ReSharper disable once CppIncompleteSwitchStatement
		switch (ClassifyPlane(boundingBox, frustum.m_planes[i]))
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

bool IsOverlapping2D(const Sphere& sphere, const BoundingBox& boundingBox)
{
	float dMin = 0;


	if (sphere.m_center.x < boundingBox.m_lowerBoundary.x)
	{
		const float diff = sphere.m_center.x - boundingBox.m_lowerBoundary.x;
		dMin += diff * diff;
	}
	else if (sphere.m_center.x > boundingBox.m_upperBoundary.x)
	{
		const float diff = sphere.m_center.x - boundingBox.m_upperBoundary.x;
		dMin += diff;
	}

	if (sphere.m_center.y < boundingBox.m_lowerBoundary.y)
	{
		const float diff = sphere.m_center.y - boundingBox.m_lowerBoundary.y;
		dMin += diff * diff;
	}
	else if (sphere.m_center.y > boundingBox.m_upperBoundary.y)
	{
		const float diff = sphere.m_center.y - boundingBox.m_upperBoundary.y;
		dMin += diff;
	}

	return dMin <= (sphere.m_radius * sphere.m_radius);
}
