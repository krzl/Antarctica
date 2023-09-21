// ReSharper disable CppInconsistentNaming
#pragma once

struct Point2DInt
{
	int32_t x = 0;
	int32_t y = 0;

	Point2DInt operator-(const Point2DInt& other) const;
	Point2DInt operator+(const Point2DInt& other) const;
	Point2DInt operator*(const int32_t value) const;
	Point2DInt operator/(const int32_t value) const;

	static Point2DInt Min(const Point2DInt& a, const Point2DInt& b);
	static Point2DInt Max(const Point2DInt& a, const Point2DInt& b);
};
