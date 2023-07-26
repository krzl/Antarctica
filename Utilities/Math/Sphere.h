#pragma once

struct Sphere
{
	Point3D m_center;
	float m_radius;

	bool Contains(const Point3D& point) const
	{
		return SquaredMag(m_center - point) <= (m_radius * m_radius);
	}
};
