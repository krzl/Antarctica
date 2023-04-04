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

void DecomposeTransform(const Transform4D& transform, Vector3D& translation, Quaternion& rotation, Vector3D& scale)
{
	translation = transform.GetTranslation();
	scale       = Vector3D(
		Magnitude(Vector3D(transform[0].xyz)),
		Magnitude(Vector3D(transform[1].xyz)),
		Magnitude(Vector3D(transform[2].xyz))
	);
	rotation.SetRotationMatrix(transform);
}

Vector3D LerpClamped(const Vector3D& a, const Vector3D& b, float alpha)
{
	alpha = Terathon::Clamp(alpha, 0.0f, 1.0f);

	return Vector3D(
		a.x * (1.0f - alpha) + b.x * alpha,
		a.y * (1.0f - alpha) + b.y * alpha,
		a.z * (1.0f - alpha) + b.z * alpha
	);
}

Quaternion SlerpClamped(const Quaternion& a, const Quaternion& b, float alpha)
{
	alpha = Terathon::Clamp(alpha, 0.0f, 1.0f);

	const double rawCos = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;

	const double cosine = abs(rawCos);
	const double angle  = acos(cosine);
	const double sine   = sin(angle);

	float ratioA;
	float ratioB;


	if (cosine < 0.9999f)
	{
		ratioA = sin((1.0f - alpha) * angle) / sine;
		ratioB = sin(alpha * angle) / sine;
	}
	else
	{
		ratioA = 1.0f - alpha;
		ratioB = alpha;
	}

	if (rawCos < 0.0f)
	{
		ratioB = -ratioB;
	}

	Quaternion q = a * ratioA + b * ratioB;
	q.Normalize();
	return q;
}
