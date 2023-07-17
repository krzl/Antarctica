#pragma once

#include "GameObjects/Component.h"

class SceneComponent : public Component
{
	DEFINE_CLASS()

	friend class GameObject;

public:

	SceneComponent() :
		m_parent(Ref<SceneComponent>()) { }

	~SceneComponent() override;

	Point3D GetLocalPosition() const
	{
		return m_position;
	}

	void SetLocalPosition(const Point3D position);

	Point3D GetWorldPosition() const;
	void    SetWorldPosition(const Point3D position);
	void    MarkDirty();

	Quaternion GetLocalRotation() const
	{
		return m_rotation;
	}

	void SetLocalRotation(const Quaternion rotation);

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

	void SetLocalScale(const Vector3D scale);

	Vector3D GetWorldScale() const;
	void     SetWorldScale(const Vector3D scale);

	const Transform4D& GetWorldTransform() const;
	const Transform4D& GetLocalTransform() const;
	const Transform4D& GetInverseWorldTransform() const;
	
	void               RemoveFromParent();

	Ref<SceneComponent> GetParent() const
	{
		return m_parent;
	}

	void SetParent(Ref<SceneComponent> parent);

	virtual float TraceRay(const RayIntersectionTester& ray, float& closestDistance) const;

protected:

	Point3D    m_position = Point3D::origin;
	Quaternion m_rotation = Quaternion::identity;
	Vector3D   m_scale    = { 1.0f, 1.0f, 1.0f };

	void OnMarkedDirty();

private:
	
	void SetParentInternal(Ref<SceneComponent> parent, Ref<SceneComponent> self);

	mutable bool m_isLocalTransformDirty  = true;
	mutable bool m_isGlobalTransformDirty = true;

	mutable Transform4D m_localTransform  = Transform4D::identity;
	mutable Transform4D m_globalTransform = Transform4D::identity;

	mutable std::optional<Transform4D> m_inverseGlobalTransform = Transform4D::identity;

	Ref<SceneComponent>              m_parent;
	std::vector<Ref<SceneComponent>> m_children;
};

CREATE_CLASS(SceneComponent)
