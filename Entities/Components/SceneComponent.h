#pragma once

#include "GameObjects/Component.h"

class SceneComponent : public Component
{
	DEFINE_CLASS()

	friend class GameObject;

public:

	SceneComponent() :
		m_parent(Ref<SceneComponent>()) { }

	Point3D GetLocalPosition() const
	{
		return m_position;
	}

	void SetLocalPosition(const Point3D position)
	{
		m_position = position;
	}

	Point3D GetWorldPosition() const;
	void    SetWorldPosition(const Point3D position);

	Quaternion GetLocalRotation() const
	{
		return m_rotation;
	}

	void SetLocalRotation(const Quaternion rotation)
	{
		m_rotation = rotation;
	}

	void SetLocalRotation(const Vector3D axis);
	void SetLocalRotation(const float x, const float y, const float z);

	Quaternion GetWorldRotation() const;
	void       SetWorldRotation(const Quaternion rotation);
	void       SetWorldRotation(const Vector3D axis);
	void       SetWorldRotation(const float x, const float y, const float z);

	Vector3D GetLocalScale() const
	{
		return m_scale;
	}

	void SetLocalScale(const Vector3D scale)
	{
		m_scale = scale;
	}

	Vector3D GetWorldScale() const;
	void     SetWorldScale(const Vector3D scale);

	Transform4D GetWorldTransform() const;
	Transform4D GetLocalTransform() const;

	Ref<SceneComponent> GetParent() const
	{
		return m_parent;
	}

protected:

	Point3D    m_position = Point3D::origin;
	Quaternion m_rotation = Quaternion::identity;
	Vector3D   m_scale    = { 1.0f, 1.0f, 1.0f };

	Ref<SceneComponent> m_parent;
};

CREATE_CLASS(SceneComponent)
