#include "stdafx.h"
#include "Point2DInt.h"

Point2DInt Point2DInt::operator-(const Point2DInt& other) const
{
	return {
		x - other.x,
		y - other.y
	};
}

Point2DInt Point2DInt::operator+(const Point2DInt& other) const
{
	return {
		x + other.x,
		y + other.y
	};
}

Point2DInt Point2DInt::operator*(const int32_t value) const
{
	return {
		x * value,
		y * value
	};
}

Point2DInt Point2DInt::operator/(const int32_t value) const
{
	return {
		x / value,
		y / value
	};
}

Point2DInt Point2DInt::Min(const Point2DInt& a, const Point2DInt& b)
{
	return Point2DInt{ ::Min(a.x, b.x), ::Min(a.y, b.y) };
}

Point2DInt Point2DInt::Max(const Point2DInt& a, const Point2DInt& b)
{
	return Point2DInt{ ::Max(a.x, b.x), ::Max(a.y, b.y) };
}
