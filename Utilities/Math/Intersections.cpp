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

	const Vector2D offset = sphere.m_center.GetPoint2D() - closestPoint;
	const float distance  = offset.x * offset.x + offset.y * offset.y;

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

	const Vector3D offset = sphere.m_center - closestPoint;
	const float distance  = Dot(offset, offset);

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

static uint32_t GetSegmentOrientation(const Point2D& a, const Point2D& b, const Point2D& c)
{
	const float value = (b.y - a.y) * (c.x - b.x) - (b.x - a.x) * (c.y - b.y);

	if (value == 0.0f)
	{
		return 0;
	}

	return value > 0 ? 1 : 2;
}

static bool IsOnSegment(const Point2D& p1, const Point2D& p2, const Point2D& p)
{
	const bool a = Min(p1.x, p2.x) <= p.x;
	const bool b = p.x <= Max(p1.x, p2.x);
	const bool c = Min(p1.y, p2.y) <= p.y;
	const bool d = p.y <= Max(p1.y, p2.y);

	return a && b && c && d;
}

bool Intersect(const Point2D& a1, const Point2D& a2, const Point2D& b1, const Point2D& b2)
{
	const uint32_t d1 = GetSegmentOrientation(a1, a2, b1);
	const uint32_t d2 = GetSegmentOrientation(a1, a2, b2);
	const uint32_t d3 = GetSegmentOrientation(b1, b2, a1);
	const uint32_t d4 = GetSegmentOrientation(b1, b2, a2);

	if (d1 != d2 && d3 != d4)
	{
		return true;
	}
	if (d1 == 0 && IsOnSegment(a1, a2, b1))
	{
		return true;
	}
	if (d2 == 0 && IsOnSegment(a1, a2, b2))
	{
		return true;
	}
	if (d3 == 0 && IsOnSegment(b1, b2, a1))
	{
		return true;
	}
	if (d4 == 0 && IsOnSegment(b1, b2, a2))
	{
		return true;
	}

	return false;
}

bool Intersect2D(const Point3D& a1, const Point3D& a2, const Point3D& b1, const Point3D& b2)
{
	return Intersect(Point2D(a1.x, a1.y), Point2D(a2.x, a2.y), Point2D(b1.x, b1.y), Point2D(b2.x, b2.y));
}

float Intersect(const Ray& ray, const Triangle& triangle)
{
	const Vector3D edge1 = triangle.m_vertices[1] - triangle.m_vertices[0];
	const Vector3D edge2 = triangle.m_vertices[2] - triangle.m_vertices[0];
	const Vector3D h     = Cross(ray.m_direction, edge2);

	const float a = Dot(edge1, h);

	if (a > -0.00001f && a < 0.00001f)
	{
		//Ray is parallel to triangle
		return -1.0f;
	}

	const float f    = 1.0f / a;
	const Vector3D s = ray.m_origin - triangle.m_vertices[0];

	const float u = f * Dot(s, h);

	if (u < 0.0f || u > 1.0f)
	{
		return -1.0f;
	}

	const Vector3D q = Cross(s, edge1);
	const float v    = f * Dot(ray.m_direction, q);

	if (v < 0.0f || v > 1.0f)
	{
		return - 1.0f;
	}

	const float t = f * Dot(edge2, q);
	if (t > 0.0001f)
	{
		return t;
	}

	return -1.0f;
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
