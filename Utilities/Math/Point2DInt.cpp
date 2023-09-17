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
