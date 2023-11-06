#pragma once

#include "Components/Component.h"

struct CameraScrollComponent : Component
{
	const float m_dragSpeed     = 0.02f;
	const float m_keyboardSpeed = 15.0f;

	const float m_heightDeltaSpeed = 0.01f;
	const float m_maxHeight        = 80.0f;
	const float m_minHeight        = 10.0f;

	bool m_isDragging = false;

	DEFINE_CLASS()
};

CREATE_CLASS(CameraScrollComponent)
