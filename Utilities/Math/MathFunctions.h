#pragma once

Quaternion DirectionToQuaternion(Vector3D direction);
Quaternion EulerToQuaternion(float x, float y, float z);

float LerpClamped(float a, float b, float alpha);

Vector3D   LerpClamped(const Vector3D& a, const Vector3D& b, float alpha);
Quaternion SlerpClamped(const Quaternion& a, const Quaternion& b, float alpha);

float DegToRad(float deg);
float RadToDeg(float rad);

void DecomposeTransform(const Transform4D& transform, Vector3D& translation, Quaternion& rotation, Vector3D& scale);

template<typename T>
float InverseLerp(T a, T b, T currentTime)
{
	return (float) (currentTime - a) / (b - a);
}

template<typename T>
T Clamp(const T& a, const T& b, const T& value)
{
	return value < a ? a : (value > b ? b : value);
}

float GetDistanceFromLineToPoint(const Vector3D& direction, const Point3D& point);

float GetDistanceSquaredFromLineToPoint(const Vector3D& direction, const Point3D& point);
