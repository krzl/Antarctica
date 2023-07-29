#pragma once

#include "Component.h"
#include "Quadtree/Quadtree.h"

struct TransformComponent : Component
{
	Point3D m_localPosition    = Point3D::origin;
	Quaternion m_localRotation = Quaternion::identity;
	Vector3D m_localScale      = { 1.0f, 1.0f, 1.0f };

	QuadtreePlacementRef m_quadtreePlacement;

	Transform4D GetWorldTransform() const
	{
		return Transform4D::MakeTranslation(m_localPosition) *
			m_localRotation.GetRotationMatrix() *
			Transform4D::MakeScale(m_localScale.x, m_localScale.y, m_localScale.z);
	}

	DEFINE_CLASS()
};

CREATE_CLASS(TransformComponent)
