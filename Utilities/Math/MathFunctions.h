#pragma once

Quaternion DirectionToQuaternion(Vector3D direction);
Quaternion EulerToQuaternion(float x, float y, float z);

float LerpClamped(float a, float b, float alpha);

Vector3D LerpClamped(const Vector3D& a, const Vector3D& b, float alpha);
Point3D LerpClamped(const Point3D& a, const Point3D& b, float alpha);
Color LerpClamped(const Color& a, const Color& b, float alpha);
Quaternion SlerpClamped(const Quaternion& a, const Quaternion& b, float alpha);

float DegToRad(float deg);
float RadToDeg(float rad);

// remap angle to (-pi, pi) interval
float RemapAngleRad(float angle);

float AngleDifference(float a, float b);

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

bool IsNaN(const Vector2D& vector);
bool IsNaN(const Vector3D& vector);
bool IsNaN(const Vector4D& vector);
bool IsNaN(const Point3D& point);

bool operator!=(const Vector2D& lhs, const Vector2D& rhs);
bool operator!=(const Vector3D& lhs, const Vector3D& rhs);
bool operator!=(const Vector4D& lhs, const Vector4D& rhs);
bool operator!=(const Point3D& lhs, const Point3D& rhs);

bool operator==(const Vector2D& lhs, const Vector2D& rhs);
bool operator==(const Vector3D& lhs, const Vector3D& rhs);
bool operator==(const Vector4D& lhs, const Vector4D& rhs);
bool operator==(const Point3D& lhs, const Point3D& rhs);
