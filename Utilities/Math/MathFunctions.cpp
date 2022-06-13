#include "stdafx.h"
#include "MathFunctions.h"


Quaternion DirectionToQuaternion(Vector3D direction)
{
	direction.Normalize();

	Vector3D xAxis = Cross(direction, Vector3D::z_unit);
	xAxis.Normalize();

	Vector3D zAxis = Cross(xAxis, direction);
	zAxis.Normalize();

	Matrix3D rotationMatrix(xAxis, direction, zAxis);

	Quaternion quaternion;
	quaternion.SetRotationMatrix(rotationMatrix);

	return quaternion;
}

Quaternion EulerToQuaternion(const float x, const float y, const float z)
{
	return Quaternion::MakeRotationX(DegToRad(x)) *
		   Quaternion::MakeRotationY(DegToRad(y)) *
		   Quaternion::MakeRotationZ(DegToRad(z));
}

float DegToRad(const float deg)
{
	return deg * Terathon::Math::pi / 180.0f;
}

float RadToDeg(const float rad)
{
	return rad * 180.0f / Terathon::Math::pi;
}
