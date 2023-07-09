#include "stdafx.h"
#include "MathFunctions.h"


Quaternion DirectionToQuaternion(Vector3D direction)
{
	direction.Normalize();

	Vector3D xAxis = Cross(Vector3D::y_unit, direction);
	xAxis.Normalize();

	Vector3D yAxis = Cross(direction, xAxis);
	yAxis.Normalize();

	Matrix3D rotationMatrix(xAxis, yAxis, direction);

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

float LerpClamped(const float a, const float b, const float alpha)
{
	return a * (1.0f - alpha) + b * alpha;
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

float GetDistanceSquaredFromLineToPoint(const Vector3D& direction, const Point3D& point)
{
	const float t = (point.x * direction.x + point.y * direction.y + point.z * direction.z) /
					(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

	Vector3D closestPointOnVector = { t * direction.x, t * direction.y, t * direction.z };

	const float distanceSquared = (closestPointOnVector.x - point.x) * (closestPointOnVector.x - point.x) +
								  (closestPointOnVector.y - point.y) * (closestPointOnVector.y - point.y) +
								  (closestPointOnVector.z - point.z) * (closestPointOnVector.z - point.z);

	return distanceSquared;
}

float GetDistanceFromLineToPoint(const Vector3D& direction, const Point3D& point)
{
	return std::sqrt(GetDistanceSquaredFromLineToPoint(direction, point));
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

Point3D LerpClamped(const Point3D& a, const Point3D& b, float alpha)
{
	alpha = Terathon::Clamp(alpha, 0.0f, 1.0f);

	return Point3D(
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
