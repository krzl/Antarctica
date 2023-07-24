#pragma once

#include "Components/Component.h"

namespace Rendering
{
	struct CameraComponent : Component
	{
		float m_fov         = 60;
		float m_farZ        = 100;
		float m_nearZ       = 0.1f;
		float m_order       = 0;

		Matrix4D m_viewMatrix;
		Matrix4D m_perspectiveMatrix;

		Frustum m_frustum;

		DEFINE_CLASS()
	};

	CREATE_CLASS(CameraComponent)
}
