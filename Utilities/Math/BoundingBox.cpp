#include "stdafx.h"
#include "BoundingBox.h"

bool BoundingBox::Contains2D(const Point2D& point) const
{
	return m_lowerBoundary.x <= point.x &&
		m_lowerBoundary.y <= point.y &&
		m_upperBoundary.x >= point.x &&
		m_upperBoundary.y >= point.y;
}

bool BoundingBox::Contains(const Point3D& point) const
{
	return m_lowerBoundary.x <= point.x &&
		m_lowerBoundary.y <= point.y &&
		m_lowerBoundary.z <= point.z &&
		m_upperBoundary.x >= point.x &&
		m_upperBoundary.y >= point.y &&
		m_upperBoundary.z >= point.z;
}

bool BoundingBox::Contains2D(const BoundingBox& other) const
{
	return Contains2D((Point2D) other.m_lowerBoundary.xy) &&
		Contains2D((Point2D) other.m_upperBoundary.xy);
}

bool BoundingBox::Contains(const BoundingBox& other) const
{
	return Contains(other.m_lowerBoundary) &&
		Contains(other.m_upperBoundary);
}

bool BoundingBox::Overlap2D(const BoundingBox& other) const
{
	return m_upperBoundary.x >= other.m_lowerBoundary.x &&
		m_upperBoundary.y >= other.m_lowerBoundary.y &&
		m_lowerBoundary.x <= other.m_upperBoundary.x &&
		m_lowerBoundary.y <= other.m_upperBoundary.y;
}

bool BoundingBox::Overlap(const BoundingBox& other) const
{
	return m_upperBoundary.x >= other.m_lowerBoundary.x &&
		m_upperBoundary.y >= other.m_lowerBoundary.y &&
		m_upperBoundary.z >= other.m_lowerBoundary.z &&
		m_lowerBoundary.x <= other.m_upperBoundary.x &&
		m_lowerBoundary.y <= other.m_upperBoundary.y &&
		m_lowerBoundary.z <= other.m_upperBoundary.z;
}

Point3D BoundingBox::GetCenter() const
{
	return (m_upperBoundary - m_lowerBoundary) / 2.0f + m_lowerBoundary;
}

void BoundingBox::Append(const Point3D point)
{
	m_lowerBoundary.x = min(m_lowerBoundary.x, point.x);
	m_lowerBoundary.y = min(m_lowerBoundary.y, point.y);
	m_lowerBoundary.z = min(m_lowerBoundary.z, point.z);
	m_upperBoundary.x = max(m_upperBoundary.x, point.x);
	m_upperBoundary.y = max(m_upperBoundary.y, point.y);
	m_upperBoundary.z = max(m_upperBoundary.z, point.z);
}

void BoundingBox::Append(const BoundingBox& other)
{
	m_lowerBoundary.x = min(m_lowerBoundary.x, other.m_lowerBoundary.x);
	m_lowerBoundary.y = min(m_lowerBoundary.y, other.m_lowerBoundary.y);
	m_lowerBoundary.z = min(m_lowerBoundary.z, other.m_lowerBoundary.z);
	m_upperBoundary.x = max(m_upperBoundary.x, other.m_upperBoundary.x);
	m_upperBoundary.y = max(m_upperBoundary.y, other.m_upperBoundary.y);
	m_upperBoundary.z = max(m_upperBoundary.z, other.m_upperBoundary.z);
}

BoundingBox BoundingBox::Transform(const Transform4D& transform) const
{
	const Point3D points[] = {
		{ m_lowerBoundary.x, m_lowerBoundary.y, m_lowerBoundary.z },
		{ m_upperBoundary.x, m_lowerBoundary.y, m_lowerBoundary.z },
		{ m_lowerBoundary.x, m_upperBoundary.y, m_lowerBoundary.z },
		{ m_upperBoundary.x, m_upperBoundary.y, m_lowerBoundary.z },
		{ m_lowerBoundary.x, m_lowerBoundary.y, m_upperBoundary.z },
		{ m_upperBoundary.x, m_lowerBoundary.y, m_upperBoundary.z },
		{ m_lowerBoundary.x, m_upperBoundary.y, m_upperBoundary.z },
		{ m_upperBoundary.x, m_upperBoundary.y, m_upperBoundary.z }
	};


	Vector3D lowerBoundary = transform * points[0];
	Vector3D upperBoundary = lowerBoundary;

	uint32_t pointCount = sizeof points / sizeof(Point3D);
	for (uint32_t i = 1; i < pointCount; ++i)
	{
		Vector3D transformed = transform * points[i];

		lowerBoundary.x = min(lowerBoundary.x, transformed.x);
		lowerBoundary.y = min(lowerBoundary.y, transformed.y);
		lowerBoundary.z = min(lowerBoundary.z, transformed.z);
		upperBoundary.x = max(upperBoundary.x, transformed.x);
		upperBoundary.y = max(upperBoundary.y, transformed.y);
		upperBoundary.z = max(upperBoundary.z, transformed.z);
	}

	return BoundingBox(lowerBoundary, upperBoundary);
}

BoundingBox BoundingBox::Scale(const float scale) const
{
	const Point3D center = GetCenter();

	return BoundingBox(
		(m_lowerBoundary - center) * scale + center,
		(m_upperBoundary - center) * scale + center
	);
}
