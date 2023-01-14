#include "stdafx.h"
#include "SceneComponent.h"

Point3D SceneComponent::GetWorldPosition() const
{
	if (GetParent().IsValid())
	{
		return GetParent()->GetWorldPosition() + GetLocalPosition();
	}
	else
	{
		return GetLocalPosition();
	}
}

void SceneComponent::SetWorldPosition(const Point3D position)
{
	if (GetParent().IsValid())
	{
		const Vector3D diff = position - GetParent()->GetWorldPosition();
		SetLocalPosition(static_cast<Point3D>(diff));
	}
	else
	{
		SetLocalPosition(position);
	}
}

void SceneComponent::SetLocalRotation(const Vector3D axis)
{
	SetLocalRotation(DirectionToQuaternion(axis));
}

void SceneComponent::SetLocalRotation(const float x, const float y, const float z)
{
	SetLocalRotation(EulerToQuaternion(x, y, z));
}

Quaternion SceneComponent::GetWorldRotation() const
{
	if (GetParent().IsValid())
	{
		return GetParent()->GetWorldRotation() * GetLocalRotation();
	}
	else
	{
		return GetLocalRotation();
	}
}

void SceneComponent::SetWorldRotation(const Quaternion rotation)
{
	if (GetParent().IsValid())
	{
		const Quaternion diff = rotation / GetParent()->GetWorldRotation();
		SetLocalRotation(diff);
	}
	else
	{
		SetLocalRotation(rotation);
	}
}

void SceneComponent::SetWorldRotation(const Vector3D axis)
{
	SetWorldRotation(DirectionToQuaternion(axis));
}

void SceneComponent::SetWorldRotation(const float x, const float y, const float z)
{
	SetWorldRotation(EulerToQuaternion(x, y, z));
}

Vector3D SceneComponent::GetWorldScale() const
{
	if (GetParent().IsValid())
	{
		return GetParent()->GetWorldScale() * GetLocalScale();
	}
	else
	{
		return GetLocalScale();
	}
}

void SceneComponent::SetWorldScale(const Vector3D scale)
{
	if (GetParent().IsValid())
	{
		const Vector3D parentScale = GetParent()->GetWorldScale();
		const Vector3D diff(scale.x / parentScale.x, scale.y / parentScale.y, scale.z / parentScale.z);
		SetLocalScale(diff);
	}
	else
	{
		SetLocalScale(scale);
	}
}

Transform4D SceneComponent::GetWorldTransform() const
{
	const Vector3D worldScale = GetWorldScale();
	return Transform4D::MakeTranslation(GetWorldPosition()) *
		   GetWorldRotation().GetRotationMatrix() *
		   Transform4D::MakeScale(worldScale.x, worldScale.y, worldScale.z);
}

Transform4D SceneComponent::GetLocalTransform() const
{
	const Vector3D localScale = GetLocalScale();
	return Transform4D::MakeTranslation(GetLocalPosition()) *
		   GetLocalRotation().GetRotationMatrix() *
		   Transform4D::MakeScale(localScale.x, localScale.y, localScale.z);
}
