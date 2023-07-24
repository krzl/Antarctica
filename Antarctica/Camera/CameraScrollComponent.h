#pragma once

#include "Components/Component.h"

struct CameraScrollComponent : Component
{
	const float m_cameraSpeed = 0.02f;

	bool m_isDragging = false;

	DEFINE_CLASS()
};

CREATE_CLASS(CameraScrollComponent)
