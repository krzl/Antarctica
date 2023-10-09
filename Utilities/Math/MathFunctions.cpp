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

float LerpClamped(const float a, const float b, float alpha)
{
	alpha = Terathon::Clamp(alpha, 0.0f, 1.0f);
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

float RemapAngleRad(float angle)
{
	angle = fmod(angle + Terathon::Math::pi, 2.0f * Terathon::Math::pi);
	if (angle < 0)
		angle += 2.0 * Terathon::Math::pi;
	return angle - Terathon::Math::pi;
}

float AngleDifference(const float a, const float b)
{
	const float diffA = abs(a - b);
	const float diffB = (2.0f * Terathon::Math::pi) - diffA;
	return diffA > diffB ? diffB : diffA;
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

Point3D GetClosestPointFromLineSegmentToPoint(const Point3D& start, const Point3D& end, const Point3D& point)
{
	const Vector3D a = point - start;
	const Vector3D dir = end - start;

	const float dot = Dot(a, dir);
	const float segmentLengthSqr = SquaredMag(dir);

	const float t = segmentLengthSqr != 0.0f ? dot / segmentLengthSqr : -1.0f;

	if (t < 0.0f)
	{
		return start;
	}
	if (t > 1.0f)
	{
		return end;
	}
	
	return Point3D(start + t * dir);
}

bool IsNaN(const Vector2D& vector)
{
	return isnan<float>(vector.x) || isnan<float>(vector.y);
}

bool IsNaN(const Vector3D& vector)
{
	return isnan<float>(vector.x) || isnan<float>(vector.y) || isnan<float>(vector.z);
}

bool IsNaN(const Vector4D& vector)
{
	return isnan<float>(vector.x) || isnan<float>(vector.y) || isnan<float>(vector.z) || isnan<float>(vector.w);
}

bool IsNaN(const Point3D& point)
{
	return isnan<float>(point.x) || isnan<float>(point.y) || isnan<float>(point.z);
}

bool operator!=(const Vector2D& lhs, const Vector2D& rhs)
{
	return !(lhs == rhs);
}

bool operator!=(const Vector3D& lhs, const Vector3D& rhs)
{
	return !(lhs == rhs);
}

bool operator!=(const Vector4D& lhs, const Vector4D& rhs)
{
	return !(lhs == rhs);
}

bool operator!=(const Point3D& lhs, const Point3D& rhs)
{
	return !(lhs == rhs);
}

bool operator==(const Vector2D& lhs, const Vector2D& rhs)
{
	return lhs.x == rhs.x &&
		lhs.y == rhs.y;
}

bool operator==(const Vector3D& lhs, const Vector3D& rhs)
{
	return lhs.x == rhs.x &&
		lhs.y == rhs.y &&
		lhs.z == rhs.z;
}

bool operator==(const Vector4D& lhs, const Vector4D& rhs)
{
	return lhs.x == rhs.x &&
		lhs.y == rhs.y &&
		lhs.z == rhs.z &&
		lhs.w == rhs.w;
}

bool operator==(const Point3D& lhs, const Point3D& rhs)
{
	return lhs.x == rhs.x &&
		lhs.y == rhs.y &&
		lhs.z == rhs.z;
}

static float Sign(const Point3D& p1, const Point3D& p2, const Point3D& p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool IsInsideTriangle(const Point3D& p, const Point3D& t1, const Point3D& t2, const Point3D& t3)
{
	const float d1 = Sign(p, t1, t2);
	const float d2 = Sign(p, t2, t3);
	const float d3 = Sign(p, t3, t1);

	const bool hasNeg = d1 < 0.0f || d2 < 0.0f || d3 < 0.0f;
	const bool hasPos = d1 > 0.0f || d2 > 0.0f || d3 > 0.0f;

	return !(hasNeg && hasPos);
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

Color LerpClamped(const Color& a, const Color& b, float alpha)
{
	alpha = Terathon::Clamp(alpha, 0.0f, 1.0f);

	return Color(
		a.r * (1.0f - alpha) + b.r * alpha,
		a.g * (1.0f - alpha) + b.g * alpha,
		a.b * (1.0f - alpha) + b.b * alpha,
		a.a * (1.0f - alpha) + b.a * alpha
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
