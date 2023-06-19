#pragma once

struct Ray
{
	Point3D  m_origin;
	Vector3D m_direction;

	Ray Transform(const Transform4D& transform) const;
};
