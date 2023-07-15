#pragma once

struct BoundingBox
{
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
