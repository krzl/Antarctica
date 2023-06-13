#include "stdafx.h"
#include "SceneComponent.h"

#include "GameObjects/GameObject.h"

SceneComponent::~SceneComponent()
{
	RemoveFromParent();
}

void SceneComponent::SetLocalPosition(const Point3D position)
{
	m_position = position;
	MarkDirty();
	if (m_owner.IsValid())
	{
		m_owner->MarkDirty();
	}
}

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

void SceneComponent::SetLocalRotation(const Quaternion rotation)
{
	m_rotation = rotation;
	MarkDirty();
	if (m_owner.IsValid())
	{
		m_owner->MarkDirty();
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

void SceneComponent::SetLocalScale(const Vector3D scale)
{
	m_scale = scale;
	MarkDirty();
	if (m_owner.IsValid())
	{
		m_owner->MarkDirty();
	}
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

const Transform4D& SceneComponent::GetWorldTransform() const
{
	if (m_isGlobalTransformDirty)
	{
		const Vector3D worldScale = GetWorldScale();

		m_globalTransform = Transform4D::MakeTranslation(GetWorldPosition()) *
							GetWorldRotation().GetRotationMatrix() *
							Transform4D::MakeScale(worldScale.x, worldScale.y, worldScale.z);

		m_isGlobalTransformDirty = false;
	}

	return m_globalTransform;
}

const Transform4D& SceneComponent::GetLocalTransform() const
{
	if (m_isLocalTransformDirty)
	{
		const Vector3D localScale = GetLocalScale();

		m_localTransform = Transform4D::MakeTranslation(GetLocalPosition()) *
						   GetLocalRotation().GetRotationMatrix() *
						   Transform4D::MakeScale(localScale.x, localScale.y, localScale.z);

		m_isLocalTransformDirty = false;
	}

	return m_localTransform;
}

void SceneComponent::RemoveFromParent()
{
	const Ref<SceneComponent> self = m_self.Cast<SceneComponent>();
	if (m_parent.IsValid())
	{
		for (uint32_t i = 0; i < m_parent->m_children.size(); ++i)
		{
			if (self == m_parent->m_children[i])
			{
				m_parent->m_children.erase(m_parent->m_children.begin() + i);
				return;
			}
		}
	}
}

auto SceneComponent::SetParent(const Ref<SceneComponent> parent) -> void
{
	SetParentInternal(parent, m_self.Cast<SceneComponent>());
}

BoundingBox SceneComponent::GetBoundingBox() const
{
	return BoundingBox(GetWorldPosition(), GetWorldPosition());
}

void SceneComponent::SetParentInternal(Ref<SceneComponent> parent, Ref<SceneComponent> self)
{
	RemoveFromParent();

	m_parent = parent;
	if (parent.IsValid())
	{
		parent->m_children.emplace_back(self);
	}
}

void SceneComponent::MarkDirty()
{
	m_isLocalTransformDirty  = true;
	m_isGlobalTransformDirty = true;

	for (Ref<SceneComponent>& child : m_children)
	{
		child->MarkDirty();
	}
}
