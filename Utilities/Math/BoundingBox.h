#pragma once

#include "Capsule.h"
#include "Ray.h"

struct BoundingBox
{
	struct RayIntersectionTester
	{
		Ray      m_ray;
		Vector3D m_rayReciprocal;

		// ReSharper disable once CppNonExplicitConvertingConstructor
		RayIntersectionTester(const Ray& ray)
		{
			m_ray = ray;

			m_rayReciprocal =
				Vector3D(1.0f / ray.m_direction.x,
						 1.0f / ray.m_direction.y,
						 1.0f / ray.m_direction.z);
		}

		float Intersect(const BoundingBox& box) const
		{
			Vector3D tMin  = (box.m_lowerBoundary - m_ray.m_origin) * m_rayReciprocal;
			Vector3D tMax  = (box.m_upperBoundary - m_ray.m_origin) * m_rayReciprocal;
			Vector3D t1    = Min(tMin, tMax);
			Vector3D t2    = Max(tMin, tMax);
			float    tNear = Max(Max((float) t1.x, (float) t1.y), (float) t1.z);
			float    tFar  = Min(Min((float) t2.x, (float) t2.y), (float) t2.z);

			if (tNear > tFar || tFar < 0.0f)
			{
				return -1.0f;
			}

			return Max(tNear, 0.0f);
		}
	};

	BoundingBox() = default;

	BoundingBox(const Vector3D& lowerBoundary, const Vector3D& upperBoundary)
		: m_lowerBoundary(lowerBoundary),
		  m_upperBoundary(upperBoundary) {}

	BoundingBox(const Point3D& lowerBoundary, const Point3D& upperBoundary)
		: m_lowerBoundary(lowerBoundary),
		  m_upperBoundary(upperBoundary) {}

	bool Contains2D(const Point2D& point) const;
	bool Contains(const Point3D& point) const;
	bool Contains2D(const BoundingBox& other) const;
	bool Contains(const BoundingBox& other) const;
	bool Overlap2D(const BoundingBox& other) const;
	bool Overlap(const BoundingBox& other) const;

	Point3D GetCenter() const;

	void Append(const Point3D point);
	void Append(const BoundingBox& other);

	BoundingBox Scale(float scale) const;
	BoundingBox Transform(const Transform4D& transform) const;

	Point3D m_lowerBoundary;
	Point3D m_upperBoundary;
};
