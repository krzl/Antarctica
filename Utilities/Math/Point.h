#pragma once

template<typename T>
struct TPoint2D
{
	T x = 0;
	T y = 0;

	TPoint2D() : TPoint2D(0, 0)
	{
		
	}

	TPoint2D(T x, T y) :
		x(x),
		y(y)
	{
	}
};

template<typename T>
struct TPoint3D
{
	T x = 0;
	T y = 0;
	T z = 0;

	TPoint3D() : TPoint3D(0, 0, 0)
	{
	}
	
	TPoint3D(T x, T y) :
		x(x),
		y(y),
		z(z)
	{
	}
};

using Point2D = TPoint2D<float>;
using Point3D = TPoint3D<float>;
using PointInt2D = TPoint2D<int32_t>;
using PointInt3D = TPoint3D<int32_t>;