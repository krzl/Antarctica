#pragma once

struct RayIntersectionTester
{
	Ray m_ray;
	Vector3D m_rayReciprocal;

	// ReSharper disable once CppNonExplicitConvertingConstructor
	RayIntersectionTester(const Ray& ray)
	{
		m_ray = ray;

		m_rayReciprocal = Vector3D(1.0f / ray.m_direction.x, 1.0f / ray.m_direction.y, 1.0f / ray.m_direction.z);
	}

	float Intersect(const BoundingBox& box) const
	{
		Vector3D tMin = (box.m_lowerBoundary - m_ray.m_origin) * m_rayReciprocal;
		Vector3D tMax = (box.m_upperBoundary - m_ray.m_origin) * m_rayReciprocal;
		Vector3D t1   = Min(tMin, tMax);
		Vector3D t2   = Max(tMin, tMax);
		float tNear   = Max(Max((float) t1.x, (float) t1.y), (float) t1.z);
		float tFar    = Min(Min((float) t2.x, (float) t2.y), (float) t2.z);

		if (tNear > tFar || tFar < 0.0f)
		{
			return -1.0f;
		}

		return Max(tNear, 0.0f);
	}
};

enum class IntersectTestResult
{
	OUTSIDE,
	INTERSECT,
	INSIDE
};

IntersectTestResult Intersect2D(const Sphere& sphere, const BoundingBox& boundingBox);
IntersectTestResult Intersect(const Sphere& sphere, const BoundingBox& boundingBox);
IntersectTestResult Intersect(const Frustum& frustum, const BoundingBox& boundingBox);

bool Intersect(const Point2D& a1, const Point2D& a2, const Point2D& b1, const Point2D& b2);
bool Intersect2D(const Point3D& a1, const Point3D& a2, const Point3D& b1, const Point3D& b2);

float Intersect(const Ray& ray, const Triangle& triangle);

bool IsOverlapping2D(const Sphere& sphere, const BoundingBox& boundingBox);
