#pragma once

Quaternion DirectionToQuaternion(Vector3D direction);
Quaternion EulerToQuaternion(float x, float y, float z);

Vector3D    LerpClamped(const Vector3D& a, const Vector3D& b, float alpha);
Quaternion  SlerpClamped(const Quaternion& a, const Quaternion& b, float alpha);

float DegToRad(float deg);
float RadToDeg(float rad);

void DecomposeTransform(const Transform4D& transform, Vector3D& translation, Quaternion& rotation, Vector3D& scale);

template<typename T>
float InverseLerp(T a, T b, T currentTime)
{
	return (float) (currentTime - a) / (b - a);
}
